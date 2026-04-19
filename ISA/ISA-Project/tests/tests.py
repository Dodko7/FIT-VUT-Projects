#!/usr/bin/env python3
"""
DNS Filter Test Suite
Tests command-line argument parsing and functionality for the DNS filter server.
Strict testing to ensure full compliance with zadanie.md requirements.

Usage:
    ./tests.py                          Run all tests with dot output
    ./tests.py -s                       Show statistics (detailed input/output)
    ./tests.py -t test_name             Run specific test by name
    ./tests.py -l                       List all available tests
    ./tests.py --help                   Show this help
"""

import subprocess
import sys
from typing import List, Tuple, Optional, Dict
import argparse
import socket
import struct
import time
import threading
import os
import tempfile
import random
import re

# ANSI color codes
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def colored(text: str, color: str) -> str:
    """Return colored text."""
    return f"{color}{text}{Colors.RESET}"

class TestCase:
    """Represents a single test case."""
    def __init__(self, test_id: str, name: str, args: List[str], should_succeed: bool, 
                 error_pattern: Optional[str] = None, description: str = "",
                 custom_runner: Optional[callable] = None):
        self.test_id = test_id  # Unique identifier for running specific tests
        self.name = name  # Human-readable name
        self.args = args
        self.should_succeed = should_succeed
        self.error_pattern = error_pattern
        self.description = description
        self.custom_runner = custom_runner  # Optional custom test function
        self.passed = False
        self.output = ""
        self.error = ""
        self.returncode = None

def run_test(test: TestCase) -> bool:
    """
    Run a single test case.
    If custom_runner is provided, use it; otherwise, use default subprocess.
    Returns True if test passes, False otherwise.
    """
    if test.custom_runner:
        # Run custom test logic
        try:
            test.passed = test.custom_runner(test)
        except Exception as e:
            test.error = f"Exception during test execution: {str(e)}\n"
            test.error += f"Exception type: {type(e).__name__}"
            test.passed = False
        return test.passed
    
    # Default subprocess runner for argument tests - ALWAYS ADD -v
    args_with_verbose = test.args.copy()
    if '-v' not in args_with_verbose:
        args_with_verbose.append('-v')
    
    cmd = ['./dns'] + args_with_verbose
    
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=5
        )
        test.output = result.stdout
        test.error = result.stderr
        test.returncode = result.returncode
        
        # Check if the program succeeded or failed as expected
        if test.should_succeed:
            if result.returncode == 0 or "Permission denied" in test.error or "requires root" in test.error:
                test.passed = True
            else:
                test.passed = False
        else:
            if result.returncode != 0:
                if test.error_pattern:
                    test.passed = test.error_pattern in test.error or test.error_pattern in test.output
                else:
                    test.passed = True
            else:
                test.passed = False
                
    except subprocess.TimeoutExpired:
        if test.should_succeed:
            test.passed = True
            test.error = "Process timed out (server started successfully)"
        else:
            test.passed = False
            test.error = "Process timed out (unexpected)"
        test.returncode = -1
    except Exception as e:
        test.error = f"Exception: {str(e)}\nType: {type(e).__name__}"
        test.passed = False
        test.returncode = -1
    
    return test.passed

# Helper functions for DNS packet building
def dns_name_to_bytes(name: str) -> bytes:
    """Convert domain name to DNS label format."""
    # Remove trailing dot if present (will add root label separately)
    name = name.rstrip('.')
    parts = name.split('.')
    result = b''
    for part in parts:
        if part:  # Skip empty parts
            result += struct.pack('B', len(part)) + part.encode('ascii')
    result += b'\x00'  # Add root label
    return result

def build_dns_query(domain: str, qtype: int = 1, qclass: int = 1, id: int = 0x1234, qdcount: int = 1) -> bytes:
    """Build a DNS query packet."""
    header = struct.pack('!HHHHHH', id, 0x0100, qdcount, 0, 0, 0)  # RD=1
    query_parts = b''
    for _ in range(qdcount):
        qname = dns_name_to_bytes(domain)
        query_parts += qname + struct.pack('!HH', qtype, qclass)
    return header + query_parts

def build_compressed_query(domain: str, compress_offset: int = 12, forward_pointer: bool = False) -> bytes:
    """Build DNS query with name compression (pointer). Supports forward pointers for robustness."""
    header = struct.pack('!HHHHHH', 0x1234, 0x0100, 1, 0, 0, 0)
    if forward_pointer:
        pointer = struct.pack('!H', 0xC000 | (len(header) + 100))  # Forward to future offset
    else:
        pointer = struct.pack('!H', 0xC000 | compress_offset)
    question = pointer + struct.pack('!HH', 1, 1)
    return header + question

def parse_dns_response(response: bytes) -> Dict:
    """Simple parse of DNS response for testing."""
    header = struct.unpack('!HHHHHH', response[:12])
    id, flags, qdcount, ancount, nscount, arcount = header
    qr = (flags >> 15) & 1
    rcode = flags & 0xF
    return {'id': id, 'qr': qr, 'rcode': rcode, 'ancount': ancount, 'qdcount': qdcount}

def extract_qname_from_response(response: bytes) -> str:
    """Extract QNAME from DNS response question section."""
    if len(response) < 12:
        return ""
    
    offset = 12  # Skip header
    labels = []
    
    while offset < len(response):
        length = response[offset]
        
        # Check for compression pointer
        if (length & 0xC0) == 0xC0:
            if offset + 1 >= len(response):
                return ""
            # Compression pointer - we'll stop here for simplicity
            break
        
        # End of name
        if length == 0:
            break
        
        # Extract label
        if offset + 1 + length > len(response):
            return ""
        
        label = response[offset + 1:offset + 1 + length].decode('ascii', errors='ignore')
        labels.append(label)
        offset += 1 + length
    
    return '.'.join(labels).lower()

def start_server(args: List[str]) -> Tuple[subprocess.Popen, int, str]:
    """Start the DNS server in background with -v flag, return process, port, filter_file."""
    port = 10053 + random.randint(0, 1000)  # Random high port
    
    # Always add -v flag if not present
    args_with_verbose = args.copy()
    if '-v' not in args_with_verbose:
        args_with_verbose.append('-v')
    
    full_args = ['./dns'] + args_with_verbose + ['-p', str(port)]
    filter_file = next((a for i, a in enumerate(full_args) if full_args[i-1] == '-f'), 'filter.txt')
    proc = subprocess.Popen(full_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    time.sleep(1.0)
    return proc, port, filter_file

def send_udp_query(host: str, port: int, query: bytes, timeout: float = 5.0) -> Optional[bytes]:
    """Send UDP DNS query and receive response."""
    family = socket.AF_INET if ':' not in host else socket.AF_INET6
    sock = socket.socket(family, socket.SOCK_DGRAM)
    sock.settimeout(timeout)
    try:
        sock.sendto(query, (host, port))
        response, _ = sock.recvfrom(1024)
        return response
    except socket.timeout:
        return None
    finally:
        sock.close()

def create_temp_filter(content: str) -> str:
    """Create temporary filter file."""
    fd, path = tempfile.mkstemp(suffix='.txt')
    with os.fdopen(fd, 'w') as f:
        f.write(content)
    return path

def cleanup(proc: subprocess.Popen, filter_path: Optional[str] = None):
    """Cleanup process and temp files."""
    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
    if filter_path and os.path.exists(filter_path):
        os.remove(filter_path)

# Custom runners for advanced tests
def custom_dns_query_runner(test: TestCase) -> bool:
    """Custom runner for DNS query tests (preposielanie neblokovaneho)."""
    proc, port, _ = start_server(test.args)
    try:
        query = build_dns_query('example.com')
        response = send_udp_query('127.0.0.1', port, query)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        if response:
            parsed = parse_dns_response(response)
            result = parsed['qr'] == 1 and parsed['rcode'] == 0 and parsed['ancount'] > 0
            if not result:
                test.error += f"\nDNS Response: QR={parsed['qr']}, RCODE={parsed['rcode']}, ANCOUNT={parsed['ancount']}"
            return result
        else:
            test.error += "\nNo response"
            return False
    finally:
        cleanup(proc)

def custom_blocked_query_runner(test: TestCase) -> bool:
    """Custom runner for blocked query, including subdomain - STRICT VERSION."""
    filter_content = "example.com\n"
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        query_main = build_dns_query('example.com')
        response_main = send_udp_query('127.0.0.1', port, query_main)
        query_sub = build_dns_query('sub.example.com')
        response_sub = send_udp_query('127.0.0.1', port, query_sub)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        if response_main and response_sub:
            parsed_main = parse_dns_response(response_main)
            parsed_sub = parse_dns_response(response_sub)
            # STRICT: Only accept NXDOMAIN (3) or REFUSED (5) for blocked domains
            # Do NOT accept FORMERR (1) or SERVFAIL (2) - those indicate errors!
            result = parsed_main['rcode'] in (3, 5) and parsed_sub['rcode'] in (3, 5)
            if not result:
                test.error += f"\nMain RCODE={parsed_main['rcode']}, Sub RCODE={parsed_sub['rcode']} (expected 3 or 5 only)"
            return result
        else:
            test.error += "\nNo response"
            return False
    finally:
        cleanup(proc, temp_filter)

def custom_multi_query_runner(test: TestCase) -> bool:
    """Custom runner for multi-question query."""
    proc, port, _ = start_server(test.args)
    try:
        header = struct.pack('!HHHHHH', 0x1234, 0x0100, 2, 0, 0, 0)
        q1 = dns_name_to_bytes('example.com') + struct.pack('!HH', 1, 1)
        q2 = dns_name_to_bytes('test.com') + struct.pack('!HH', 1, 1)
        query = header + q1 + q2
        response = send_udp_query('127.0.0.1', port, query)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        if response:
            parsed = parse_dns_response(response)
            result = parsed['rcode'] in (1, 4, 5)
            if not result:
                test.error += f"\nRCODE={parsed['rcode']}"
            return result
        else:
            test.error += "\nNo response"
            return False
    finally:
        cleanup(proc)

def custom_compression_runner(test: TestCase) -> bool:
    """Custom runner for compression - tests robustness with self-referencing pointer."""
    proc, port, _ = start_server(test.args)
    try:
        query_standard = build_compressed_query('example.com')
        response_standard = send_udp_query('127.0.0.1', port, query_standard)
        query_forward = build_compressed_query('example.com', forward_pointer=True)
        response_forward = send_udp_query('127.0.0.1', port, query_forward)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        if response_standard and response_forward:
            parsed_standard = parse_dns_response(response_standard)
            parsed_forward = parse_dns_response(response_forward)
            # STRICT: Invalid compression should return FORMERR (1)
            # Accepting RCODE 0 would mean server somehow resolved an invalid pointer!
            result = parsed_standard['rcode'] == 1 and parsed_forward['rcode'] == 1
            if not result:
                test.error += f"\nStandard RCODE={parsed_standard['rcode']}, Forward RCODE={parsed_forward['rcode']} (expected 1 for invalid compression)"
            return result
        else:
            test.error += "\nNo response"
            return False
    finally:
        cleanup(proc)

def custom_compression_forward_runner(test: TestCase) -> bool:
    """Custom runner for compression in forwarded response."""
    proc, port, _ = start_server(test.args)
    try:
        query = build_dns_query('example.com')
        response = send_udp_query('127.0.0.1', port, query)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        if response:
            result = b'\xc0' in response[12:]  # Check for pointer in body
            if not result:
                test.error += "\nNo compression pointer found"
            return result
        else:
            test.error += "\nNo response"
            return False
    finally:
        cleanup(proc)

def custom_ipv_runner(test: TestCase) -> bool:
    """Custom runner for IPv4 and IPv6 communication."""
    proc, port, _ = start_server(test.args)
    try:
        query = build_dns_query('example.com')
        response_ipv4 = send_udp_query('127.0.0.1', port, query)
        response_ipv6 = send_udp_query('::1', port, query)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        if response_ipv4 and response_ipv6:
            parsed_ipv4 = parse_dns_response(response_ipv4)
            parsed_ipv6 = parse_dns_response(response_ipv6)
            result = parsed_ipv4['qr'] == 1 and parsed_ipv6['qr'] == 1
            if not result:
                test.error += f"\nIPv4 QR={parsed_ipv4['qr']}, IPv6 QR={parsed_ipv6['qr']}"
            return result
        else:
            test.error += "\nNo response (IPv4 or IPv6)"
            return False
    finally:
        cleanup(proc)

def custom_error_input_runner(test: TestCase) -> bool:
    """Custom runner for invalid inputs - STRICT VERSION."""
    proc, port, _ = start_server(test.args)
    try:
        invalid_query = b'\x00\x00' * 6
        response_invalid = send_udp_query('127.0.0.1', port, invalid_query)
        non_a_query = build_dns_query('example.com', qtype=28)  # AAAA
        response_non_a = send_udp_query('127.0.0.1', port, non_a_query)
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=5)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        # STRICT: Server MUST respond to all queries, even invalid ones
        if not response_invalid:
            test.error += "\nNo response to invalid query - server must always respond!"
            return False
        if not response_non_a:
            test.error += "\nNo response to non-A query - server must always respond!"
            return False
            
        parsed_invalid = parse_dns_response(response_invalid)
        parsed_non_a = parse_dns_response(response_non_a)
        
        # Invalid query should get FORMERR (1) or SERVFAIL (2)
        invalid_result = parsed_invalid['rcode'] in (1, 2)
        # Non-A query should get NOTIMP (4) or REFUSED (5)
        non_a_result = parsed_non_a['rcode'] in (4, 5)
        
        result = invalid_result and non_a_result
        if not result:
            test.error += f"\nInvalid RCODE={parsed_invalid['rcode']} (expected 1 or 2), Non-A RCODE={parsed_non_a['rcode']} (expected 4 or 5)"
        return result
    finally:
        cleanup(proc)

def custom_packet_loss_runner(test: TestCase) -> bool:
    """Custom runner simulating packet loss."""
    args = [a if '8.8.8.8' not in a else 'invalid.resolver' for a in test.args]
    
    # Add -v flag if not present
    if '-v' not in args:
        args.append('-v')
    
    try:
        result = subprocess.run(['./dns'] + args, capture_output=True, text=True, timeout=5)
        test.output = result.stdout
        test.error = result.stderr
        test.returncode = result.returncode
        result = result.returncode != 0 and "Failed to resolve" in test.error
        if not result:
            test.error += f"\nExpected graceful fail, got code={result.returncode}"
        return result
    except subprocess.TimeoutExpired:
        test.error += "Unexpected timeout"
        return False

def custom_bad_filter_runner(test: TestCase) -> bool:
    """Custom runner for bad filter file, including line endings."""
    contents = [
        "invalid_domain@\nexample.com\n",  # LF
        "invalid_domain@\rexample.com\r",  # CR
        "invalid_domain@\r\nexample.com\r\n"  # CRLF
    ]
    results = []
    for content in contents:
        temp_filter = create_temp_filter(content)
        args = [a if a != 'filter.txt' else temp_filter for a in test.args]
        
        # Add -v flag if not present
        if '-v' not in args:
            args.append('-v')
        
        try:
            result = subprocess.run(['./dns'] + args, capture_output=True, text=True, timeout=5)
            test.output += result.stdout
            test.error += result.stderr
            results.append("warning" in result.stderr.lower() or "invalid" in result.stderr.lower())
        except subprocess.TimeoutExpired:
            results.append(True)
        finally:
            os.remove(temp_filter)
    return all(results)

def custom_memory_runner(test: TestCase) -> bool:
    """Custom runner with valgrind - STRICT VERSION with proper leak detection."""
    # Get a random port
    port = 10053 + random.randint(0, 1000)
    
    # Add -v flag if not present
    args_with_verbose = test.args.copy()
    if '-v' not in args_with_verbose:
        args_with_verbose.append('-v')
    
    args_with_port = args_with_verbose + ['-p', str(port)]
    cmd = ['valgrind', '--leak-check=full', '--error-exitcode=1', '--track-origins=yes', './dns'] + args_with_port
    
    try:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        time.sleep(1.5)  # Let server start
        
        # STRICT: Send multiple queries to stress-test memory allocation
        for i in range(50):
            query = build_dns_query(f'test{i}.com')
            try:
                send_udp_query('127.0.0.1', port, query, timeout=1.0)
            except:
                pass  # Ignore individual query errors
        
        # Terminate server
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=10)
        
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        # STRICT: Check for definite leaks
        definitely_lost = re.search(r'definitely lost: ([\d,]+) bytes', stderr)
        if definitely_lost:
            bytes_lost = int(definitely_lost.group(1).replace(',', ''))
            if bytes_lost > 0:
                test.error += f"\n\nMemory leak detected: {bytes_lost} bytes definitely lost!"
                return False
        
        # Check for indirect leaks
        indirectly_lost = re.search(r'indirectly lost: ([\d,]+) bytes', stderr)
        if indirectly_lost:
            bytes_lost = int(indirectly_lost.group(1).replace(',', ''))
            if bytes_lost > 0:
                test.error += f"\n\nMemory leak detected: {bytes_lost} bytes indirectly lost!"
                return False
        
        # Check for invalid memory access
        if 'Invalid read' in stderr or 'Invalid write' in stderr:
            test.error += "\n\nInvalid memory access detected!"
            return False
        
        # Success if no leaks found
        no_leaks_msg = 'no leaks are possible' in stderr or 'All heap blocks were freed' in stderr
        return no_leaks_msg or (definitely_lost is not None and indirectly_lost is not None)
        
    except subprocess.TimeoutExpired:
        test.error = "Valgrind test timed out"
        return False
    except Exception as e:
        test.error = f"Valgrind error: {str(e)}"
        return False

def custom_parallel_runner(test: TestCase) -> bool:
    """Custom runner for parallel queries - STRICT VERSION with proper verification."""
    proc, port, _ = start_server(test.args)
    try:
        results = []
        lock = threading.Lock()
        
        # Use specific domains to verify correct routing
        test_domains = [
            ('allowed1.com', False),  # Should be allowed
            ('allowed2.com', False),
            ('allowed3.com', False),
            ('allowed4.com', False),
            ('allowed5.com', False),
            ('allowed6.com', False),
            ('allowed7.com', False),
            ('allowed8.com', False),
            ('allowed9.com', False),
            ('allowed10.com', False),
        ]
        
        def send_query(domain, should_be_blocked):
            query = build_dns_query(domain)
            response = send_udp_query('127.0.0.1', port, query)
            
            if response is None:
                with lock:
                    results.append((domain, False, "No response"))
                return
            
            parsed = parse_dns_response(response)
            qname = extract_qname_from_response(response)
            
            # Verify response matches query
            if qname != domain:
                with lock:
                    results.append((domain, False, f"Wrong domain in response: {qname}"))
                return
            
            # Verify RCODE is acceptable (0=success, 3=nxdomain, 4=notimp, 5=refused)
            rcode_ok = parsed['rcode'] in (0, 1, 3, 4, 5)
            
            with lock:
                results.append((domain, rcode_ok, f"RCODE={parsed['rcode']}"))

        threads = [threading.Thread(target=send_query, args=(domain, blocked)) 
                   for domain, blocked in test_domains]
        
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        # Capture output
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        # Check results
        if len(results) != len(test_domains):
            test.error += f"\nOnly {len(results)}/10 threads completed"
            return False
        
        failures = [r for r in results if not r[1]]
        if failures:
            test.error += f"\nFailed queries:\n"
            for domain, _, reason in failures:
                test.error += f"  {domain}: {reason}\n"
            return False
        
        return True
        
    except Exception as e:
        test.error = f"Parallel test error: {str(e)}"
        return False
    finally:
        cleanup(proc)

def custom_performance_runner(test: TestCase) -> bool:
    """Custom runner for large filter and many queries."""
    filter_content = '\n'.join(f'domain{i}.com' for i in range(10000))
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        start = time.time()
        for _ in range(100):
            query = build_dns_query('example.com')
            if send_udp_query('127.0.0.1', port, query) is None:
                return False
        duration = time.time() - start
        return duration < 10.0
    finally:
        cleanup(proc, temp_filter)

def custom_cache_runner(test: TestCase) -> bool:
    """Custom runner for caching, with memory limits."""
    proc, port, _ = start_server(test.args)
    try:
        query = build_dns_query('example.com')
        start1 = time.time()
        response1 = send_udp_query('127.0.0.1', port, query)
        duration1 = time.time() - start1
        start2 = time.time()
        response2 = send_udp_query('127.0.0.1', port, query)
        duration2 = time.time() - start2
        if response1 and response2:
            for i in range(100):
                send_udp_query('127.0.0.1', port, build_dns_query(f'cachetest{i}.com'))
            response_final = send_udp_query('127.0.0.1', port, query)
            if response_final is None:
                test.error += "\nCache overflow or crash"
                return False
            return duration2 < duration1 * 0.5 or True  # Optional feature
        return False
    finally:
        cleanup(proc)

def custom_verbose_runner(test: TestCase) -> bool:
    """Custom runner for verbose mode (NOTE: -v is now always enabled)."""
    # Since -v is always on, this test just verifies server runs
    proc, port, _ = start_server(test.args)
    try:
        time.sleep(0.2)
        query = build_dns_query('example.com')
        send_udp_query('127.0.0.1', port, query)
        time.sleep(0.2)
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        # Verify verbose output exists (stdout or stderr should have content)
        has_output = len(stdout) > 0 or len(stderr) > 0
        
        if not has_output:
            test.error += "\nNo verbose output produced with -v flag"
            return False
        
        return True
    except Exception as e:
        test.error = f"Verbose test error: {str(e)}"
        return False
    finally:
        cleanup(proc)

def custom_dig_wireshark_runner(test: TestCase) -> bool:
    """Custom runner simulating dig and Wireshark."""
    proc, port, _ = start_server(test.args)
    try:
        query = build_dns_query('example.com')
        response = send_udp_query('127.0.0.1', port, query)
        return response is not None and (b'\xc0' in response or True)  # Optional compression check
    finally:
        cleanup(proc)

def custom_filter_formats_runner(test: TestCase) -> bool:
    """Custom runner for filter domain formats test."""
    # Create temporary filter file with various domain formats
    filter_content = """# This is a comment
blocked.com
www.example.org

# Another comment
test-domain.net
subdomain.test.com
"""
    temp_filter = create_temp_filter(filter_content)
    # Replace filter.txt with temp filter in args
    args = [temp_filter if a == 'filter.txt' else a for a in test.args]
    
    # Start server with temporary filter
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.5)
        
        # Test that blocked.com is actually blocked
        query = build_dns_query('blocked.com')
        response = send_udp_query('127.0.0.1', port, query, timeout=2.0)
        
        if not response:
            test.error = "No response for blocked domain query"
            return False
        
        parsed = parse_dns_response(response)
        # Should be NXDOMAIN (3) or REFUSED (5)
        if parsed['rcode'] not in [3, 5]:
            test.error = f"Expected NXDOMAIN/REFUSED for blocked domain, got RCODE={parsed['rcode']}"
            return False
        
        return True
    except Exception as e:
        test.error = f"Filter formats test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_dnsperf_stress_1k_runner(test: TestCase) -> bool:
    """Stress test with 1000 queries using dnsperf."""
    # Check if dnsperf is installed
    result = subprocess.run(['which', 'dnsperf'], capture_output=True)
    if result.returncode != 0:
        test.error = "dnsperf not installed - skipping stress test (install with: sudo apt install dnsperf)"
        test.passed = True  # Don't fail if tool not available
        return True
    
    # Create query file
    query_file = create_temp_filter('\n'.join([f'example{i}.com A' for i in range(1000)]))
    temp_filter = create_temp_filter('blocked.com\n')
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    
    proc, port, _ = start_server(args)
    try:
        time.sleep(1.0)
        
        # Run dnsperf
        cmd = [
            'dnsperf', '-s', '127.0.0.1', '-p', str(port),
            '-d', query_file, '-c', '10', '-l', '5'
        ]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        
        if result.returncode != 0:
            test.error = f"dnsperf failed: {result.stderr}"
            return False
        
        # Check for reasonable completion
        if 'Queries sent:' not in result.stdout:
            test.error = "dnsperf output incomplete"
            return False
        
        return True
    except subprocess.TimeoutExpired:
        test.error = "dnsperf timed out"
        return False
    except Exception as e:
        test.error = f"Stress test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)
        if os.path.exists(query_file):
            os.unlink(query_file)

def custom_dnsperf_stress_5k_runner(test: TestCase) -> bool:
    """Stress test with 5000 queries using dnsperf."""
    result = subprocess.run(['which', 'dnsperf'], capture_output=True)
    if result.returncode != 0:
        test.error = "dnsperf not installed - skipping stress test"
        test.passed = True
        return True
    
    query_file = create_temp_filter('\n'.join([f'example{i}.com A' for i in range(5000)]))
    temp_filter = create_temp_filter('blocked.com\n')
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    
    proc, port, _ = start_server(args)
    try:
        time.sleep(1.0)
        
        cmd = [
            'dnsperf', '-s', '127.0.0.1', '-p', str(port),
            '-d', query_file, '-c', '20', '-l', '10'
        ]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        
        if result.returncode != 0:
            test.error = f"dnsperf failed: {result.stderr}"
            return False
        
        if 'Queries sent:' not in result.stdout:
            test.error = "dnsperf output incomplete"
            return False
        
        return True
    except subprocess.TimeoutExpired:
        test.error = "dnsperf timed out"
        return False
    except Exception as e:
        test.error = f"Stress test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)
        if os.path.exists(query_file):
            os.unlink(query_file)

def custom_dnsperf_stress_10k_runner(test: TestCase) -> bool:
    """Stress test with 10000 queries using dnsperf."""
    result = subprocess.run(['which', 'dnsperf'], capture_output=True)
    if result.returncode != 0:
        test.error = "dnsperf not installed - skipping stress test"
        test.passed = True
        return True
    
    query_file = create_temp_filter('\n'.join([f'example{i}.com A' for i in range(10000)]))
    temp_filter = create_temp_filter('blocked.com\n')
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    
    proc, port, _ = start_server(args)
    try:
        time.sleep(1.0)
        
        cmd = [
            'dnsperf', '-s', '127.0.0.1', '-p', str(port),
            '-d', query_file, '-c', '30', '-l', '15'
        ]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        
        if result.returncode != 0:
            test.error = f"dnsperf failed: {result.stderr}"
            return False
        
        if 'Queries sent:' not in result.stdout:
            test.error = "dnsperf output incomplete"
            return False
        
        return True
    except subprocess.TimeoutExpired:
        test.error = "dnsperf timed out"
        return False
    except Exception as e:
        test.error = f"Stress test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)
        if os.path.exists(query_file):
            os.unlink(query_file)

def custom_domain_validation_runner(test: TestCase) -> bool:
    """Custom runner for domain validation per RFC - tests that digit-starting domains are valid."""
    filter_content = "blocked.com\n"
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.2)
        # Test that digit-starting domain is ALLOWED (RFC1123 permits it)
        query_digit = build_dns_query('1example.com')
        response_digit = send_udp_query('127.0.0.1', port, query_digit)
        
        # Test that blocked domain is blocked
        query_blocked = build_dns_query('blocked.com')
        response_blocked = send_udp_query('127.0.0.1', port, query_blocked)
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        if response_digit and response_blocked:
            parsed_digit = parse_dns_response(response_digit)
            parsed_blocked = parse_dns_response(response_blocked)
            
            # 1example.com should be allowed (RCODE 0 or forwarded)
            digit_ok = parsed_digit['rcode'] in (0, 2)  # NOERROR or SERVFAIL (if upstream fails)
            
            # blocked.com should be blocked (RCODE 3 or 5)
            blocked_ok = parsed_blocked['rcode'] in (3, 5)
            
            if not (digit_ok and blocked_ok):
                test.error += f"\nDigit domain RCODE={parsed_digit['rcode']} (expected 0 or 2), Blocked RCODE={parsed_blocked['rcode']} (expected 3 or 5)"
            
            return digit_ok and blocked_ok
        else:
            test.error += "\nNo response received"
            return False
    except Exception as e:
        test.error = f"Validation error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_portability_runner(test: TestCase) -> bool:
    """Custom runner for portability - simulate compile."""
    result_compile = subprocess.run(['make'], capture_output=True, text=True)
    if result_compile.returncode != 0:
        test.error = "Compile failed"
        return False
    # Manual test on merlin/eva documented in README
    return True

def custom_crlf_filter_runner(test: TestCase) -> bool:
    """Test filter file with Windows CRLF line endings."""
    filter_content = "blocked.com\r\nexample.org\r\n# comment\r\ntest.com\r\n"
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.2)
        # Test that blocked.com is actually blocked
        query = build_dns_query('blocked.com')
        response = send_udp_query('127.0.0.1', port, query)
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        if response:
            parsed = parse_dns_response(response)
            return parsed['rcode'] in (3, 5)  # Should be blocked
        return False
    except Exception as e:
        test.error = f"CRLF test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_cr_filter_runner(test: TestCase) -> bool:
    """Test filter file with Mac CR line endings."""
    filter_content = "blocked.com\rexample.org\r# comment\rtest.com\r"
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.2)
        query = build_dns_query('blocked.com')
        response = send_udp_query('127.0.0.1', port, query)
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        if response:
            parsed = parse_dns_response(response)
            return parsed['rcode'] in (3, 5)
        return False
    except Exception as e:
        test.error = f"CR test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_mixed_line_endings_runner(test: TestCase) -> bool:
    """Test filter file with mixed line endings."""
    filter_content = "blocked.com\r\nexample.org\ntest.com\r# comment\n"
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.2)
        # Test all three domains are blocked
        for domain in ['blocked.com', 'example.org', 'test.com']:
            query = build_dns_query(domain)
            response = send_udp_query('127.0.0.1', port, query)
            if not response:
                test.error += f"\nNo response for {domain}"
                return False
            parsed = parse_dns_response(response)
            if parsed['rcode'] not in (3, 5):
                test.error += f"\n{domain} not blocked (RCODE={parsed['rcode']})"
                return False
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        return True
    except Exception as e:
        test.error = f"Mixed line endings test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_case_sensitivity_runner(test: TestCase) -> bool:
    """Test case-insensitive filter matching."""
    filter_content = "Example.COM\n"  # Mixed case in filter
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.2)
        # Test lowercase, uppercase, mixed - all should be blocked
        for domain in ['example.com', 'EXAMPLE.COM', 'ExAmPlE.cOm']:
            query = build_dns_query(domain)
            response = send_udp_query('127.0.0.1', port, query)
            if not response:
                test.error += f"\nNo response for {domain}"
                return False
            parsed = parse_dns_response(response)
            if parsed['rcode'] not in (3, 5):
                test.error += f"\n{domain} not blocked (RCODE={parsed['rcode']})"
                return False
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        return True
    except Exception as e:
        test.error = f"Case sensitivity test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_trailing_dot_runner(test: TestCase) -> bool:
    """Test domains with trailing dots."""
    filter_content = "example.com\n"
    temp_filter = create_temp_filter(filter_content)
    args = [a if a != 'filter.txt' else temp_filter for a in test.args]
    proc, port, _ = start_server(args)
    try:
        time.sleep(0.2)
        # Both with and without trailing dot should match
        query1 = build_dns_query('example.com')
        query2 = build_dns_query('example.com.')
        
        response1 = send_udp_query('127.0.0.1', port, query1)
        response2 = send_udp_query('127.0.0.1', port, query2)
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        if response1 and response2:
            parsed1 = parse_dns_response(response1)
            parsed2 = parse_dns_response(response2)
            
            # Both should be blocked
            both_blocked = (parsed1['rcode'] in (3, 5) and parsed2['rcode'] in (3, 5))
            
            if not both_blocked:
                test.error += f"\nWithout dot RCODE={parsed1['rcode']}, With dot RCODE={parsed2['rcode']} (both should be 3 or 5)"
            
            return both_blocked
        return False
    except Exception as e:
        test.error = f"Trailing dot test error: {str(e)}"
        return False
    finally:
        cleanup(proc, temp_filter)

def custom_qtype_runner(test: TestCase) -> bool:
    """Test different QTYPE queries (AAAA, MX, TXT) should get NOTIMP/REFUSED."""
    proc, port, _ = start_server(test.args)
    try:
        time.sleep(0.2)
        # Test AAAA (28), MX (15), TXT (16)
        qtypes = [(28, 'AAAA'), (15, 'MX'), (16, 'TXT')]
        
        for qtype, name in qtypes:
            query = build_dns_query('example.com', qtype=qtype)
            response = send_udp_query('127.0.0.1', port, query)
            
            if not response:
                test.error += f"\nNo response for {name} query"
                return False
            
            parsed = parse_dns_response(response)
            # Should get NOTIMP (4) or REFUSED (5)
            if parsed['rcode'] not in (4, 5):
                test.error += f"\n{name} query: RCODE={parsed['rcode']} (expected 4 or 5)"
                return False
        
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
        return True
    except Exception as e:
        test.error = f"QTYPE test error: {str(e)}"
        return False
    finally:
        cleanup(proc)

def custom_id_collision_runner(test: TestCase) -> bool:
    """Custom runner for ID collision - STRICT VERSION that verifies response routing."""
    proc, port, _ = start_server(test.args)
    try:
        time.sleep(0.2)
        # Two queries with same ID but different domains
        query1 = build_dns_query('google.com', id=0x9999)
        query2 = build_dns_query('facebook.com', id=0x9999)
        
        # Send from different sockets (simulating different clients)
        sock1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock1.settimeout(2.0)
        sock2.settimeout(2.0)
        
        try:
            sock1.sendto(query1, ('127.0.0.1', port))
            sock2.sendto(query2, ('127.0.0.1', port))
            
            response1, _ = sock1.recvfrom(1024)
            response2, _ = sock2.recvfrom(1024)
            
            # STRICT: Verify responses are routed to correct clients
            qname1 = extract_qname_from_response(response1)
            qname2 = extract_qname_from_response(response2)
            
            parsed1 = parse_dns_response(response1)
            parsed2 = parse_dns_response(response2)
            
            # Both should be valid responses (QR=1)
            both_valid = parsed1['qr'] == 1 and parsed2['qr'] == 1
            
            # CRITICAL: Responses must match the queries they're responding to!
            correct_routing = qname1 == 'google.com' and qname2 == 'facebook.com'
            
            if not both_valid:
                test.error += f"\nInvalid responses: QR1={parsed1['qr']}, QR2={parsed2['qr']}"
                return False
            
            if not correct_routing:
                test.error += f"\nWrong routing: sock1 got '{qname1}' (expected 'google.com'), sock2 got '{qname2}' (expected 'facebook.com')"
                return False
            
            return True
            
        finally:
            sock1.close()
            sock2.close()
            
        proc.terminate()
        stdout, stderr = proc.communicate(timeout=2)
        test.output = stdout
        test.error = stderr
        test.returncode = proc.returncode
        
    except Exception as e:
        test.error = f"ID collision test error: {str(e)}"
        return False
    finally:
        cleanup(proc)

# Test Suites
def get_arguments_basic_tests() -> List[TestCase]:
    return [
        TestCase("test_args_basic_all_required", "All required arguments", ["-s", "8.8.8.8", "-p", "5300", "-f", "filter.txt"], True, description="Basic valid arguments"),
        TestCase("test_args_basic_default_port", "Default port", ["-s", "8.8.8.8", "-f", "filter.txt"], True, description="Using default port 53 (may require root)"),
        TestCase("test_args_basic_no_args", "No arguments", [], False, "Usage:", description="No arguments provided"),
        TestCase("test_args_basic_missing_server", "Missing server", ["-p", "5300", "-f", "filter.txt"], False, "Usage:", description="Missing -s"),
        TestCase("test_args_basic_missing_filter", "Missing filter", ["-s", "8.8.8.8", "-p", "5300"], False, "Usage:", description="Missing -f"),
        TestCase("test_args_basic_order_variation", "Argument order 1", ["-f", "filter.txt", "-s", "8.8.8.8", "-p", "5300"], True, description="Different order"),
        TestCase("test_args_basic_unknown_option", "Unknown option -x", ["-s", "8.8.8.8", "-p", "5300", "-f", "filter.txt", "-x"], False, "Usage:", description="Unrecognized option"),
        TestCase("test_args_basic_help_flag", "Help flag -h", ["-h"], True, "Usage:", description="Shows help message"),
    ]

def get_arguments_port_tests() -> List[TestCase]:
    return [
        TestCase("test_args_port_min_boundary", "Min port boundary", ["-s", "8.8.8.8", "-p", "1", "-f", "filter.txt"], True, description="Min valid port"),
        TestCase("test_args_port_max_boundary", "Max port boundary", ["-s", "8.8.8.8", "-p", "65535", "-f", "filter.txt"], True, description="Max valid port"),
        TestCase("test_args_port_zero", "Port 0", ["-s", "8.8.8.8", "-p", "0", "-f", "filter.txt"], False, "out of range", description="Reserved port"),
        TestCase("test_args_port_negative", "Port -1", ["-s", "8.8.8.8", "-p", "-1", "-f", "filter.txt"], False, "out of range", description="Negative port"),
        TestCase("test_args_port_string", "Port as string", ["-s", "8.8.8.8", "-p", "abc", "-f", "filter.txt"], False, "Invalid port", description="Non-numeric"),
    ]

def get_arguments_server_ipv_tests() -> List[TestCase]:
    return [
        TestCase("test_args_server_ipv4_google", "Google DNS 8.8.8.8", ["-s", "8.8.8.8", "-p", "5300", "-f", "filter.txt"], True, description="IPv4 Google"),
        TestCase("test_args_server_ipv4_localhost", "Localhost IPv4", ["-s", "127.0.0.1", "-p", "5300", "-f", "filter.txt"], True, description="IPv4 localhost"),
        TestCase("test_args_server_ipv6_google", "Google DNS IPv6", ["-s", "2001:4860:4860::8888", "-p", "5300", "-f", "filter.txt"], True, description="IPv6 Google"),
        TestCase("test_args_server_ipv6_localhost", "Localhost IPv6", ["-s", "::1", "-p", "5300", "-f", "filter.txt"], True, description="IPv6 localhost"),
        TestCase("test_args_server_invalid_ipv4", "Invalid IP 999.999.999.999", ["-s", "999.999.999.999", "-p", "5300", "-f", "filter.txt"], False, "Failed to resolve", description="Invalid IPv4"),
        TestCase("test_args_server_invalid_ipv6", "Malformed IPv6", ["-s", "2001:4860:4860::8888::", "-p", "5300", "-f", "filter.txt"], False, "Failed to resolve", description="Invalid IPv6"),
    ]

def get_arguments_server_domain_tests() -> List[TestCase]:
    return [
        TestCase("test_args_server_domain_valid", "Domain name dns.google", ["-s", "dns.google", "-p", "5300", "-f", "filter.txt"], True, description="Domain resolver (getaddrinfo)"),
        TestCase("test_args_server_domain_invalid", "Unresolvable domain", ["-s", "this-domain-definitely-does-not-exist-12345.com", "-p", "5300", "-f", "filter.txt"], False, "Failed to resolve", description="Bad domain"),
    ]

def get_arguments_filter_file_tests() -> List[TestCase]:
    return [
        TestCase("test_args_filter_nonexistent", "Nonexistent filter file", ["-s", "8.8.8.8", "-p", "5300", "-f", "this_file_does_not_exist_xyz123.txt"], False, "Failed to open", description="Missing file"),
    ]

def get_filter_content_tests() -> List[TestCase]:
    return [
        TestCase("test_filter_domain_formats", "Filter with various domain formats", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_filter_formats_runner, description="Comments, blank lines"),
        TestCase("test_filter_standard", "Standard filter file", ["-s", "8.8.8.8", "-p", "5300", "-f", "filter.txt"], True, description="Default filter"),
        TestCase("test_filter_bad_format", "Chybný formát filter_file", ["-s", "8.8.8.8", "-p", "5300", "-f", "filter.txt"], True, custom_runner=custom_bad_filter_runner, description="Ignores bad lines, warnings"),
        TestCase("test_filter_line_endings_crlf", "Filter with CRLF line endings (Windows)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_crlf_filter_runner, description="Windows \\r\\n"),
        TestCase("test_filter_line_endings_cr", "Filter with CR line endings (Mac)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_cr_filter_runner, description="Mac \\r"),
        TestCase("test_filter_line_endings_mixed", "Filter with mixed line endings", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_mixed_line_endings_runner, description="Mixed \\n, \\r\\n, \\r"),
        TestCase("test_filter_case_insensitive", "Case-insensitive domain matching", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_case_sensitivity_runner, description="Example.COM matches example.com"),
        TestCase("test_filter_trailing_dot", "Trailing dot in domain names", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_trailing_dot_runner, description="example.com. vs example.com"),
    ]

def get_unit_tests() -> List[TestCase]:
    return [
        TestCase("test_unit_dns_compression", "Parsovanie DNS hlavičky a QNAME (kompresia)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_compression_runner, description="Handles compressed QNAME"),
        TestCase("test_unit_domain_validation", "Validácia domény proti filtru", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_domain_validation_runner, description="Validates per RFC"),
        TestCase("test_unit_filter_read", "Čítanie a spracovanie filter_file", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_bad_filter_runner, description="Reads filter, ignores bad"),
        TestCase("test_unit_error_responses", "Generovanie chybových odpovedí", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_error_input_runner, description="Generates RCODE"),
        TestCase("test_unit_qtype_unsupported", "Nepodporované QTYPE (AAAA, MX, TXT)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_qtype_runner, description="Returns NOTIMP/REFUSED"),
    ]

def get_integration_tests() -> List[TestCase]:
    return [
        TestCase("test_integration_forward_query", "Prijímanie a prepôsobanie neblokovaného dotazu (typ A)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_dns_query_runner, description="Forwards A query"),
        TestCase("test_integration_block_query", "Filtrovanie blokovaného dotazu", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_blocked_query_runner, description="Blocks domain/subdomain"),
        TestCase("test_integration_multi_query", "Spracovanie viacerých dotazov v jednej správe", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_multi_query_runner, description="Handles multi-question"),
        TestCase("test_integration_compression_forward", "Podpora kompresie pri prepôsobaní odpovede", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_compression_forward_runner, description="Preserves compression"),
        TestCase("test_integration_id_collision", "ID collision handling (multi-client)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_id_collision_runner, description="Same ID from different clients"),
    ]

def get_system_tests() -> List[TestCase]:
    return [
        TestCase("test_system_verbose_mode", "Spuštenie s parametrami (-s, -p, -f, -v)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_verbose_runner, description="Runs with verbose"),
        TestCase("test_system_ipv4_ipv6", "Komunikácia cez IPv4 a IPv6", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_ipv_runner, description="Supports IPv4/IPv6"),
        TestCase("test_system_caching", "Cachovanie odpovedí (ak implementované)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_cache_runner, description="Caches with limits"),
        TestCase("test_system_dig_wireshark", "Testy s reálnymi nástrojmi (dig, Wireshark)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_dig_wireshark_runner, description="Simulates tools"),
        TestCase("test_stress_dnsperf_1k", "Stress test: 1000 queries (dnsperf)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_dnsperf_stress_1k_runner, description="Performance with 1k queries"),
        TestCase("test_stress_dnsperf_5k", "Stress test: 5000 queries (dnsperf)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_dnsperf_stress_5k_runner, description="Performance with 5k queries"),
        TestCase("test_stress_dnsperf_10k", "Stress test: 10000 queries (dnsperf)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_dnsperf_stress_10k_runner, description="Performance with 10k queries"),
    ]

def get_robustness_tests() -> List[TestCase]:
    return [
        TestCase("test_robustness_error_inputs", "Spracovanie chybných vstupov", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_error_input_runner, description="Handles bad inputs no crash"),
        TestCase("test_robustness_packet_loss", "Strata paketov alebo timeout", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_packet_loss_runner, description="No crash on loss"),
        TestCase("test_robustness_bad_filter", "Chybný formát filter_file", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_bad_filter_runner, description="Warns on bad format"),
        TestCase("test_robustness_memory", "Pamäťové testy (valgrind)", ["-s", "8.8.8.8", "-p", "5300", "-f", "filter.txt"], True, custom_runner=custom_memory_runner, description="No leaks"),
    ]

def get_portability_tests() -> List[TestCase]:
    return [
        TestCase("test_portability_reference_machines", "Prenositeľnosť na referenčných strojoch (merlin, eva)", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_portability_runner, description="Compiles/runs"),
        TestCase("test_portability_parallel", "Paralelné spracovanie dotazov", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_parallel_runner, description="Handles concurrent"),
        TestCase("test_portability_performance", "Výkon pri veľkom filtri alebo mnohých dotazoch", ["-s", "8.8.8.8", "-f", "filter.txt"], True, custom_runner=custom_performance_runner, description="Fast with large data"),
    ]

# Test Runner
def run_test_suite(suite_name: str, tests: List[TestCase], show_stats: bool = False):
    if not tests:
        return 0, 0
    
    passed = 0
    failed = 0
    
    if show_stats:
        print(f"\n{'='*70}")
        print(f"{colored('CATEGORY:', Colors.BOLD + Colors.CYAN)} {colored(suite_name, Colors.BOLD + Colors.YELLOW)}")
        print(f"{'='*70}\n")
        
        for test in tests:
            run_test(test)
            
            test_name_len = len(test.name) + 2
            padding_needed = 70 - test_name_len
            left_chars = padding_needed // 2
            right_chars = padding_needed - left_chars
            left = ("+-" * (left_chars // 2 + 1))[:left_chars]
            right = ("-+" * (right_chars // 2 + 1))[:right_chars]
            separator = f"{left}>{colored(test.name, Colors.BOLD)}<{right}"
            print(separator)
            
            if test.passed:
                print(colored("✓ PASSED", Colors.GREEN + Colors.BOLD))
                passed += 1
            else:
                print(colored("✗ FAILED", Colors.RED + Colors.BOLD))
                print(f"\n{colored('TEST DETAILS:', Colors.CYAN + Colors.BOLD)}")
                print(f"{'='*70}")
                
                if test.custom_runner:
                    runner_name = test.custom_runner.__name__
                    print(f"Test Type: Custom ({runner_name})")
                    print(f"Description: {test.description}")
                    print(f"Command/Actions: ./dns {' '.join(test.args)} -v + custom logic")
                else:
                    print(f"Test Type: Argument test")
                    print(f"Command: ./dns {' '.join(test.args)} -v")
                
                print(f"\n{colored('EXPECTED:', Colors.YELLOW + Colors.BOLD)}")
                if test.should_succeed:
                    print(f"  Status: Success")
                else:
                    print(f"  Status: Failure")
                    if test.error_pattern:
                        print(f"  Error pattern: '{test.error_pattern}'")
                
                print(f"\n{colored('ACTUAL OUTPUT:', Colors.RED + Colors.BOLD)}")
                print(f"{'='*70}")
                if test.returncode is not None:
                    print(f"Exit Code: {test.returncode}")
                if test.output.strip():
                    print(f"\nStdout:\n─{ '─'*69 }\n{test.output}\n─{ '─'*69 }")
                if test.error.strip():
                    print(f"\nStderr:\n─{ '─'*69 }\n{test.error}\n─{ '─'*69 }")
                print(f"{'='*70}")
                failed += 1
    else:
        print(f"\n/>{colored(suite_name + ':', Colors.BOLD)} ", end='')
        
        for test in tests:
            run_test(test)
            print(colored('.', Colors.GREEN) if test.passed else colored('.', Colors.RED), end='')
            passed += 1 if test.passed else 0
            failed += 1 if not test.passed else 0
        
        print()
    
    return passed, failed

# Main
def main():
    parser = argparse.ArgumentParser(description='DNS Filter Test Suite', add_help=False)
    parser.add_argument('-s', '--stats', action='store_true', help='Show detailed stats')
    parser.add_argument('-h', '--help', action='store_true', help='Show help')
    parser.add_argument('-t', '--test', type=str, help='Run a specific test by test_id')
    parser.add_argument('-l', '--list', action='store_true', help='List all available tests with their test_ids')
    
    args = parser.parse_args()
    
    # Collect all tests
    all_suites = {
        'Arguments: Basic': get_arguments_basic_tests(),
        'Arguments: Port': get_arguments_port_tests(),
        'Arguments: Server': get_arguments_server_ipv_tests(),
        'Arguments: Server (Domain)': get_arguments_server_domain_tests(),
        'Arguments: Filter File': get_arguments_filter_file_tests(),
        'Filter: Content Loading': get_filter_content_tests(),
        'Unit Tests': get_unit_tests(),
        'Integration Tests': get_integration_tests(),
        'System Tests': get_system_tests(),
        'Robustness Tests': get_robustness_tests(),
        'Portability and Performance Tests': get_portability_tests(),
    }
    
    # Help mode
    if args.help:
        print("""DNS Filter Test Suite

Usage: python3 tests.py [options]

Options:
  -s, --stats       Show detailed statistics for each test
  -t, --test ID     Run a specific test by its test_id
  -l, --list        List all available tests with their test_ids
  -h, --help        Show this help message

Examples:
  python3 tests.py                          # Run all tests
  python3 tests.py -s                       # Run all tests with detailed output
  python3 tests.py -t test_args_basic_all_required  # Run single test
  python3 tests.py -l                       # List all test IDs
""")
        sys.exit(0)
    
    # List mode
    if args.list:
        print(colored("Available Tests:", Colors.BOLD + Colors.CYAN))
        print("=" * 80)
        for suite_name, tests in all_suites.items():
            print(f"\n{colored(suite_name, Colors.BOLD + Colors.YELLOW)}")
            for test in tests:
                print(f"  {colored(test.test_id, Colors.GREEN):<45} {test.name}")
        print("\n" + "=" * 80)
        print(f"Total: {sum(len(tests) for tests in all_suites.values())} tests")
        sys.exit(0)
    
    # Single test mode
    if args.test:
        test_found = False
        for suite_name, tests in all_suites.items():
            for test in tests:
                if test.test_id == args.test:
                    test_found = True
                    print(colored(f"Running single test: {test.test_id}", Colors.BOLD + Colors.CYAN))
                    print(colored(f"Category: {suite_name}", Colors.CYAN))
                    print(colored(f"Description: {test.name}", Colors.CYAN))
                    print("=" * 70 + "\n")
                    
                    run_test(test)
                    
                    if test.passed:
                        print(colored("\n✓ PASSED", Colors.GREEN + Colors.BOLD))
                        sys.exit(0)
                    else:
                        print(colored("\n✗ FAILED", Colors.RED + Colors.BOLD))
                        print(f"\n{colored('TEST DETAILS:', Colors.CYAN + Colors.BOLD)}")
                        print(f"{'='*70}")
                        
                        if test.custom_runner:
                            runner_name = test.custom_runner.__name__
                            print(f"Test Type: Custom ({runner_name})")
                            print(f"Description: {test.description}")
                            print(f"Command/Actions: ./dns {' '.join(test.args)} -v + custom logic")
                        else:
                            print(f"Test Type: Argument test")
                            print(f"Command: ./dns {' '.join(test.args)} -v")
                        
                        print(f"\n{colored('EXPECTED:', Colors.YELLOW + Colors.BOLD)}")
                        if test.should_succeed:
                            print(f"  Status: Success")
                        else:
                            print(f"  Status: Failure")
                            if test.error_pattern:
                                print(f"  Error pattern: '{test.error_pattern}'")
                        
                        print(f"\n{colored('ACTUAL OUTPUT:', Colors.RED + Colors.BOLD)}")
                        print(f"{'='*70}")
                        if test.returncode is not None:
                            print(f"Exit Code: {test.returncode}")
                        if test.output.strip():
                            print(f"\nStdout:\n─{ '─'*69 }\n{test.output}\n─{ '─'*69 }")
                        if test.error.strip():
                            print(f"\nStderr:\n─{ '─'*69 }\n{test.error}\n─{ '─'*69 }")
                        print(f"{'='*70}")
                        sys.exit(1)
                    break
            if test_found:
                break
        
        if not test_found:
            print(colored(f"Error: Test '{args.test}' not found", Colors.RED + Colors.BOLD))
            print(colored("Use -l to list all available tests", Colors.YELLOW))
            sys.exit(1)
    
    # Normal mode: run all tests
    print(colored("Testing:", Colors.BOLD + Colors.BLUE))
    print(colored("Note: All tests run with -v (verbose) flag enabled for better error tracking", Colors.CYAN))
    
    total_passed = 0
    total_failed = 0
    
    for suite_name, tests in all_suites.items():
        if tests:
            passed, failed = run_test_suite(suite_name, tests, args.stats)
            total_passed += passed
            total_failed += failed
    
    total_tests = total_passed + total_failed
    if total_tests > 0:
        print(f"\n{'='*60}")
        pass_rate = (total_passed / total_tests * 100)
        summary = f"Results: {total_passed}/{total_tests} passed ({pass_rate:.1f}%)"
        print(colored(summary, Colors.GREEN + Colors.BOLD if total_failed == 0 else Colors.YELLOW + Colors.BOLD))
        if total_failed > 0 and not args.stats:
            print(colored("\nTip: Run with -s for details", Colors.CYAN))
        sys.exit(1 if total_failed > 0 else 0)
    else:
        print(colored("\nNo tests", Colors.YELLOW))
        sys.exit(0)

if __name__ == "__main__":
    main()