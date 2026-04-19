# Import necessary modules for parsing, XML generation, and system interaction
from lark import Lark, UnexpectedCharacters, UnexpectedToken, Visitor, Tree, Transformer
from xml.etree import ElementTree as ET
import argparse  # For parsing command-line arguments
import sys       # For system-level operations like stdin and exit codes
import re        # For regular expression handling (e.g., extracting comments)

# Define function to handle command-line argument parsing
def parse_arguments():
    """Parse command-line arguments and handle help option."""
    # Initialize argument parser with custom description and disable default help
    arg_parser = argparse.ArgumentParser(description="SOL25 Parser", add_help=False)
    # Add optional help flag with description
    arg_parser.add_argument(
        "-h", "--help",
        action="store_true",
        help="SOL25 Parser - Help\nParameters:\n"
    )
    # Parse known arguments and capture any unknowns
    args, unknown = arg_parser.parse_known_args()

    # Check for invalid combination of help flag with other arguments
    if args.help and len(sys.argv) > 2:
        print("Error: Incorrect arguments", file=sys.stderr)
        sys.exit(10)  # Exit with error code for invalid arguments

    # Handle help flag alone, ensuring no unknown arguments are present
    if args.help:
        if unknown:
            print("Error: Incorrect arguments", file=sys.stderr)
            sys.exit(10)
        print("SOL25 Parser : Help message\nParameters: --help / -h\n")
        sys.exit(0)  # Exit cleanly after showing help

    # Check for any unexpected arguments
    if unknown:
        print("Error: Incorrect arguments", file=sys.stderr)
        sys.exit(10)

# Define the SOL25 grammar as a raw string
# This grammar specifies the syntax rules for the SOL25 language using Lark
SOL25_GRAMMAR = r"""
    # Program can consist of zero or more class definitions
    ?program: class program
            | 
    # Class definition: 'class' keyword, class ID, parent ID, and methods
    class: "class" CID ":" CID "{" method "}"
    # Method can be a selector followed by a block, optionally followed by more methods
    method: selector block method
            |
    # Selector can be a simple ID or a keyword selector with tail
    selector: ID 
            | SEL selector_tail
    # Selector tail allows chaining of keyword selectors
    selector_tail: SEL selector_tail
            |
    # Block definition with parameters and statements
    block: "[" block_par "|" block_stat "]"
    # Block parameters are colon-prefixed IDs
    block_par: COLON_ID block_par
            |
    # Block statements are assignments or empty
    block_stat: ID ":=" expr "." block_stat 
            | 
    # Expression consists of a base followed by an optional tail
    expr: expr_base expr_tail
    # Expression tail can be an ID or a selector expression
    expr_tail: ID 
            | expr_sel
    # Selector expression for keyword messages with arguments
    expr_sel: SEL expr_base expr_sel
            |
    # Base expression types: literals, variables, blocks, etc.
    expr_base: INT 
            | STR 
            | ID 
            | CID 
            | block 
            | "(" expr ")" 
            | SELF 
            | SUPER 
            | NIL 
            | TRUE 
            | FALSE
            | CLASS

    # Reserved keywords for special values
    SELF: "self"
    SUPER: "super"
    NIL: "nil"
    TRUE: "true"
    FALSE: "false"
    CLASS: "class"
    
    # Regular expressions for identifiers and tokens
    ID: /(?!self|super|nil|true|false|class)[a-z_][a-zA-Z0-9_]*/ 
    SEL: /(?!self|super|nil|true|false|class)[a-z_][a-zA-Z0-9_]*:/
    COLON_ID: /:(?!self|super|nil|true|false|class)[a-z_][a-zA-Z0-9_]*/
    
    CID: /[A-Z][a-zA-Z0-9]*/
    INT: /[+-]?[0-9]+/
    STR: /'(?:[\x20-&\x28-\[\]-~\t]|\\['n\\])(?:[\x20-&\x28-\[\]-~\t]|\\['n\\])*'/
    
    # Ignore whitespace and comments
    %import common.WS
    %ignore WS
    COMMENT: /"[^"]*"/
    %ignore COMMENT
"""

# Define class for semantic analysis of SOL25 code
class SOL25SemanticsVisitor(Visitor):
    """Visitor class for semantic validation of SOL25 syntax trees."""

    # Initialize semantic visitor with built-in classes and tracking structures
    def __init__(self):
        # Dictionary to store user-defined classes
        self.classes = {}
        # Predefined built-in classes with their parents and methods
        self.builtin_classes = {
            "Object": {
                "parent": None,
                "methods": {"equalTo:", "asString", "new", "vysl", "vysl:"}
            },
            "Nil": {
                "parent": "Object",
                "methods": {"new"}
            },
            "True": {
                "parent": "Object",
                "methods": {"ifTrue:", "ifFalse:", "ifTrue:ifFalse:", "new"}
            },
            "False": {
                "parent": "Object",
                "methods": {"ifTrue:", "ifFalse:", "ifTrue:ifFalse:", "new"}
            },
            "Integer": {
                "parent": "Object",
                "methods": {
                    "plus:", "minus:", "times:", "divide:", "greaterThan:",
                    "lessThan:", "equalTo:", "asString", "from:", "new"
                }
            },
            "String": {
                "parent": "Object",
                "methods": {
                    "concatenateWith:", "print", "asString", "length", "read", "new"
                }
            },
            "Block": {
                "parent": "Object",
                "methods": {
                    "value", "value:", "value:value:", "value:value:value:",
                    "whileTrue:", "whileFalse:", "new", "ifTrue:", "ifFalse:",
                    "ifTrue:ifFalse:"
                }
            }
        }
        # Stack of current block scopes
        self.current_blocks = []
        # Stack of current class context
        self.current_class = []
        # List to store semantic errors
        self.errors = []
        # Set of used class IDs
        self.used_cids = set()
        # List of used (class, selector) pairs
        self.used_selectors = []

    # Visit nodes in the syntax tree based on their type
    def visit(self, tree):
        """Dispatch tree traversal based on node type."""
        # Handle specific node types with dedicated methods
        if tree.data == "class":
            self.class_(tree)
        elif tree.data == "block":
            self.block(tree)
        elif tree.data == "block_par":
            self.block_par(tree)
        elif tree.data == "block_stat":
            self.block_stat(tree)
        elif tree.data == "expr":
            self.expr(tree)
        elif tree.data == "expr_sel":
            self.expr_sel(tree)
        else:
            # Recursively visit all child nodes if no specific handler
            for child in tree.children:
                if isinstance(child, Tree):
                    self.visit(child)

    # Process the program root node
    def program(self, tree):
        """Visit all child nodes of the program."""
        # Iterate through all children to process class definitions
        for child in tree.children:
            if isinstance(child, Tree):
                self.visit(child)

    # Handle class definitions and their methods
    def class_(self, tree):
        """Analyze class definition and collect its methods."""
        class_name = tree.children[0].value  # Extract class name
        parent_name = tree.children[1].value  # Extract parent class name
        methods = {}  # Dictionary to store method definitions
        # Check if parent class is defined
        if parent_name not in self.classes and parent_name not in self.builtin_classes:
            self.errors.append((32, f"Undefined superclass {parent_name}"))

        # Nested function to recursively collect methods from method nodes
        def collect_methods(node):
            if isinstance(node, Tree) and node.data == "method" and node.children:
                selector_node = node.children[0]
                selector = selector_node.children[0].value  # Start with first part of selector
                # Build full selector if it has a tail
                if len(selector_node.children) > 1:
                    tail = selector_node.children[1]
                    while tail and tail.children:
                        selector += tail.children[0].value
                        tail = tail.children[1] if len(tail.children) > 1 else None
                methods[selector] = node.children[1]  # Store block for the selector
                # Recurse if more methods follow
                if len(node.children) > 2:
                    collect_methods(node.children[2])

        # Collect methods from the class body
        for child in tree.children[2:]:
            collect_methods(child)

        # Define class structure with parent and methods
        class_def = {"parent": parent_name, "methods": methods}
        # Handle redefinition or new class
        if class_name in self.classes:
            self.classes[class_name].append(class_def)  # Allow multiple definitions (to check later)
        else:
            self.classes[class_name] = [class_def]

        # Push class context and visit children
        self.current_class.append(class_name)
        for child in tree.children:
            if isinstance(child, Tree):
                self.visit(child)
        self.current_class.pop()  # Pop class context after processing

    # Process block nodes and their parameters
    def block(self, tree):
        """Validate block parameters and scope."""
        block_par = tree.children[0]  # Extract parameter node

        # Nested function to collect block parameters
        def collect_params(node, params_list):
            for child in node.children:
                if isinstance(child, Tree):
                    collect_params(child, params_list)
                elif hasattr(child, 'type') and child.type == "COLON_ID":
                    params_list.append(child.value[1:])  # Strip colon from parameter name

        params_list = []  # List to store parameter names
        collect_params(block_par, params_list)
        params_set = set(params_list)  # Convert to set to check duplicates
        # Check for duplicate parameters
        if len(params_list) != len(params_set):
            self.errors.append((35, "Duplicate parameters in block"))

        # Create new scope for block with parameters and variables
        new_scope = {"params": params_set, "vars": set()}
        self.current_blocks.append(new_scope)  # Push scope onto stack

        # Visit all child nodes of the block
        for child in tree.children:
            if isinstance(child, Tree):
                self.visit(child)

        self.current_blocks.pop()  # Pop scope after processing

    # Process block parameter nodes
    def block_par(self, tree):
        """Visit child nodes of block parameters."""
        # Recursively visit all parameter-related nodes
        for child in tree.children:
            if isinstance(child, Tree):
                self.visit(child)

    # Handle variable assignments within blocks
    def block_stat(self, tree):
        """Check variable assignments for scope conflicts."""
        if tree.children:  # If there’s an assignment
            # Ensure a scope exists
            if not self.current_blocks:
                self.errors.append((99, "Internal error: No scope available in block_stat"))
                return
            var_name = tree.children[0].value  # Get variable name
            current_scope = self.current_blocks[-1]  # Access current scope
            # Check for collision with parameters
            if var_name in current_scope["params"]:
                self.errors.append((34, f"Variable {var_name} collides with parameter"))
            else:
                current_scope["vars"].add(var_name)  # Add variable to scope
            # Visit all child nodes (e.g., expression)
            for child in tree.children:
                if isinstance(child, Tree):
                    self.visit(child)

    # Analyze expressions and their components
    def expr(self, tree):
        """Validate expression bases and tails."""
        expr_base = tree.children[0]  # Base part of the expression
        expr_tail = tree.children[1] if len(tree.children) > 1 else None  # Optional tail

        # Check the base for variable or class usage
        if isinstance(expr_base, Tree) and expr_base.data == "expr_base" and expr_base.children:
            if not isinstance(expr_base.children[0], Tree):
                base_token = expr_base.children[0]
                if base_token.type == "CID":
                    self.used_cids.add(base_token.value)  # Track used class ID
                elif base_token.type == "ID":
                    var_name = base_token.value
                    is_defined = False
                    # Check if variable is defined in any scope
                    for scope in reversed(self.current_blocks):
                        if var_name in scope["params"] or var_name in scope["vars"]:
                            is_defined = True
                            break
                    if not is_defined:
                        self.errors.append((32, f"Undefined variable {var_name}"))

        # Handle expression tail (method calls)
        if (expr_tail and isinstance(expr_tail, Tree) and
                expr_tail.data == "expr_tail" and expr_tail.children):
            if isinstance(expr_tail.children[0], Tree) and expr_tail.children[0].data == "expr_sel":
                self.expr_sel(expr_tail.children[0], expr_base)  # Process selector expression
            elif not isinstance(expr_base.children[0], Tree):
                selector = expr_tail.children[0].value  # Get selector name
                if (isinstance(expr_base, Tree) and
                        expr_base.data == "expr_base" and expr_base.children):
                    if not isinstance(expr_base.children[0], Tree):
                        base_token = expr_base.children[0]
                        if base_token.type == "CID":
                            class_name = base_token.value
                            self.used_selectors.append((class_name, selector))  # Track class method usage
                        elif base_token.type == "SELF":
                            if self.current_class:
                                class_name = self.current_class[-1]
                                self.used_selectors.append((class_name, selector))  # Track self method usage
                            else:
                                self.errors.append((99, "Internal error: No class context for SELF"))

        # Visit all non-selector child nodes
        for child in tree.children:
            if isinstance(child, Tree) and child.data != "expr_sel":
                self.visit(child)

    # Process selector expressions with arguments
    def expr_sel(self, tree, parent_base=None):
        """Build and validate selector expressions."""
        # Nested function to construct selector string
        def build_selector(node, selector=""):
            if hasattr(node, 'children') and node.children:
                if not isinstance(node.children[0], Tree) and node.children[0].type == "SEL":
                    selector += node.children[0].value  # Add keyword part
                for child in node.children:
                    if isinstance(child, Tree) and child.data == "expr_sel":
                        selector = build_selector(child, selector)  # Recurse for more parts
            return selector

        current_selector = build_selector(tree)  # Build full selector

        # Determine receiver class for method calls
        receiver_class = None
        if parent_base:
            if (isinstance(parent_base, Tree) and
                    parent_base.data == "expr_base" and parent_base.children):
                base_token = parent_base.children[0]
                if not isinstance(base_token, Tree):
                    if base_token.type == "CID":
                        receiver_class = base_token.value
                    elif base_token.type == "SELF":
                        if self.current_class:
                            receiver_class = self.current_class[-1]
                        else:
                            self.errors.append((99, "Internal error: No class context for SELF"))
            elif (isinstance(parent_base, Tree) and
                    parent_base.data == "expr" and parent_base.children):
                expr_base = parent_base.children[0]
                if (isinstance(expr_base, Tree) and
                        expr_base.data == "expr_base" and expr_base.children):
                    base_token = expr_base.children[0]
                    if not isinstance(base_token, Tree) and base_token.type == "CID":
                        receiver_class = base_token.value

        # Track method usage if receiver and selector are identified
        if current_selector and receiver_class:
            self.used_selectors.append((receiver_class, current_selector))

        # Process arguments in selector expression
        current_base = parent_base
        for child in getattr(tree, 'children', []):
            if isinstance(child, Tree):
                if child.data == "expr_base":
                    current_base = child
                    if current_base.children and not isinstance(current_base.children[0], Tree):
                        if current_base.children[0].type == "CID":
                            self.used_cids.add(current_base.children[0].value)
                        elif current_base.children[0].type == "ID":
                            var_name = current_base.children[0].value
                            is_defined = False
                            for scope in reversed(self.current_blocks):
                                if var_name in scope["params"] or var_name in scope["vars"]:
                                    is_defined = True
                                    break
                            if not is_defined:
                                self.errors.append((32, f"Undefined variable {var_name}"))
                    if (current_base.children and
                            isinstance(current_base.children[0], Tree) and
                            current_base.children[0].data == "expr"):
                        self.expr(current_base.children[0])
                elif child.data == "expr_sel":
                    self.expr_sel(child, current_base)
                else:
                    self.visit(child)

    # Retrieve all methods for a given class including inherited ones
    def _get_all_methods(self, class_name):
        all_methods = set()  # Set to store unique method names
        if class_name in self.classes:
            class_def = self.classes[class_name][0]
            all_methods.update(class_def["methods"].keys()) # Add class methods
            parent = class_def["parent"] # Recurse for parent methods
            if parent:
                all_methods.update(self._get_all_methods(parent))
        elif class_name in self.builtin_classes:
            all_methods.update(self.builtin_classes[class_name]["methods"]) # Add built-in methods
            parent = self.builtin_classes[class_name]["parent"]
            if parent:
                all_methods.update(self._get_all_methods(parent))
        return all_methods

    # Perform semantic checks on the parsed tree
    def check(self):
        """Validate semantic rules and report errors."""
        # Nested function to collect parameters from block
        def collect_params(node, params_list):
            for child in node.children:
                if isinstance(child, Tree):
                    collect_params(child, params_list)
                elif hasattr(child, 'type') and child.type == "COLON_ID":
                    params_list.append(child.value[1:])

        # Check for undefined class IDs
        for cid in self.used_cids:
            if cid not in self.classes and cid not in self.builtin_classes:
                self.errors.append((32, f"Undefined class {cid}"))

        # Check for class redefinition
        for class_name, definitions in self.classes.items():
            if len(definitions) > 1:
                self.errors.append((35, f"Class {class_name} redefined"))

        # Check for circular inheritance
        self._check_circular_inheritance()

        # Verify presence of Main class with run method
        if ("Main" not in self.classes or
                "run" not in self.classes["Main"][0]["methods"]):
            self.errors.append((31, "Missing Main class with run method"))
        else:
            run_block = self.classes["Main"][0]["methods"]["run"]
            block_par = run_block.children[0]
            params = []
            collect_params(block_par, params)
            if params:
                self.errors.append((33, "Method run must have no parameters"))

        # Check method arity in all classes
        for class_name, definitions in self.classes.items():
            for class_def in definitions:
                for selector, block in class_def["methods"].items():
                    expected_arity = selector.count(":")  # Count colons for arity
                    block_par = block.children[0]
                    params = []
                    collect_params(block_par, params)
                    actual_arity = len(params)
                    if expected_arity != actual_arity:
                        self.errors.append((
                            33,
                            f"Arity mismatch in method {selector}: expected {expected_arity}, got {actual_arity}"
                        ))

        # Validate method calls
        for class_name, selector in self.used_selectors:
            if class_name in self.classes or class_name in self.builtin_classes:
                all_methods = self._get_all_methods(class_name)
                if selector and selector not in all_methods:
                    self.errors.append((32, f"Undefined method {selector} for class {class_name}"))

        # Report first error if any exist
        if self.errors:
            error_code, error_msg = min(self.errors, key=lambda x: x[0])
            print(f"Semantic error: {error_msg}", file=sys.stderr)
            sys.exit(error_code)

    # Check for circular inheritance in class hierarchy
    def _check_circular_inheritance(self):
        """Detect circular inheritance using depth-first search."""
        # Depth-first search to detect cycles
        def dfs(class_name, visited, stack):
            if class_name in stack:
                return True  # Cycle detected
            if class_name in visited:
                return False  # Already checked, no cycle
            visited.add(class_name)
            stack.add(class_name)
            parent = self.classes.get(class_name, [{}])[0].get("parent")
            if parent and parent in self.classes:
                if dfs(parent, visited, stack):
                    return True
            stack.remove(class_name)
            return False

        visited = set()
        for class_name in self.classes:
            if dfs(class_name, visited, set()):
                print(f"Semantic error: Circular inheritance detected with {class_name}", file=sys.stderr)
                sys.exit(35)

# Define class for transforming SOL25 syntax trees to XML
class SOL25XMLTransformer(Transformer):
    """Transformer class to convert SOL25 syntax trees to XML."""

    # Initialize transformer with optional first comment
    def __init__(self, first_comment=None):
        """Set up transformer with root and optional description."""
        self.root = None  # Root of the XML tree
        self.first_comment = first_comment  # Optional comment for program description

    # Transform the program into an XML root element
    def program(self, items):
        """Create the root program element with optional description."""
        attributes = {"language": "SOL25"}  # Set language attribute
        if self.first_comment:
            attributes["description"] = self.first_comment  # Add description if provided
        self.root = ET.Element("program", attributes)  # Create root element
        self._process_program(items)  # Process program contents
        return self.root

    # Process program items recursively
    def _process_program(self, items):
        """Append class elements to the program root."""
        for item in items:
            if isinstance(item, Tree) and item.data == "class":
                class_elem = self.class_(item)  # Transform class node
                self.root.append(class_elem)  # Add to root
            elif isinstance(item, Tree) and item.data == "program":
                self._process_program(item.children)  # Recurse into program nodes

    # Handle class node transformation
    def class_(self, tree):
        """Transform class definition into XML element."""
        class_name = tree.children[0].value  # Get class name
        parent_name = tree.children[1].value  # Get parent class name
        class_elem = ET.Element("class", {"name": class_name, "parent": parent_name})  # Create class element

        # Find the first method node
        method_node = None
        for child in tree.children:
            if isinstance(child, Tree) and child.data == "method":
                method_node = child
                break

        # Process methods if present
        if method_node:
            self._process_methods(method_node, class_elem)
        return class_elem

    # Process method nodes within a class
    def _process_methods(self, method_node, class_elem):
        """Append method elements to the class element."""
        current_node = method_node
        # Iterate through chained method nodes
        while (current_node and isinstance(current_node, Tree) and
               current_node.data == "method"):
            if current_node.children:
                method_elem = self.method(current_node)  # Transform method node
                if method_elem:
                    class_elem.append(method_elem)  # Add to class element
            # Move to next method if present
            if (len(current_node.children) > 2 and
                    isinstance(current_node.children[2], Tree) and
                    current_node.children[2].data == "method"):
                current_node = current_node.children[2]
            else:
                break

    # Transform method node into XML
    def method(self, tree):
        """Create method element with selector and block."""
        if not tree.children:
            return None  # No content to process
        selector_node = tree.children[0]
        selector = self._build_selector(selector_node)  # Build method selector
        method_elem = ET.Element("method", {"selector": selector})  # Create method element
        block_node = tree.children[1]
        block_elem = self.block(block_node)  # Transform block
        method_elem.append(block_elem)  # Add block to method
        return method_elem

    # Build method selector string
    def _build_selector(self, selector_node):
        """Construct selector string from selector node."""
        selector = ""
        if selector_node.children[0].type == "ID":
            selector = selector_node.children[0].value  # Simple ID selector
        elif selector_node.children[0].type == "SEL":
            selector = selector_node.children[0].value  # Start with keyword selector
            if len(selector_node.children) > 1:
                tail = selector_node.children[1]
                selector += self._build_selector_tail(tail)  # Append tail if present
        return selector

    # Build selector tail recursively
    def _build_selector_tail(self, tail_node):
        """Append selector parts from tail nodes."""
        if not tail_node.children:
            return ""  # No tail to process
        selector_part = tail_node.children[0].value  # Add current part
        if len(tail_node.children) > 1:
            next_tail = tail_node.children[1]
            selector_part += self._build_selector_tail(next_tail)  # Recurse for more parts
        return selector_part

    # Transform block node into XML
    def block(self, tree):
        """Create block element with parameters and assignments."""
        block_par = tree.children[0]  # Parameter node
        params = []  # List to store parameters
        self._collect_params(block_par, params)  # Collect parameters
        arity = len(params)  # Number of parameters
        block_elem = ET.Element("block", {"arity": str(arity)})  # Create block element

        # Add parameter elements
        for i, param_name in enumerate(params, 1):
            param_elem = ET.Element("parameter", {"order": str(i), "name": param_name})
            block_elem.append(param_elem)

        # Process statements if present
        if len(tree.children) > 1:
            block_stat = tree.children[1]
            if isinstance(block_stat, Tree) and block_stat.data == "block_stat":
                assigns = self._process_block_stat(block_stat)  # Transform statements
                for assign_elem in assigns:
                    block_elem.append(assign_elem)  # Add assignments to block

        return block_elem

    # Collect block parameters
    def _collect_params(self, node, params):
        """Gather parameter names from block parameter node."""
        for child in node.children:
            if isinstance(child, Tree) and child.data == "block_par":
                self._collect_params(child, params)  # Recurse into parameter nodes
            elif hasattr(child, 'type') and child.type == "COLON_ID":
                params.append(child.value[1:])  # Add parameter name (strip colon)

    # Process block statements into assignments
    def _process_block_stat(self, block_stat_node):
        """Transform block statements into assignment elements."""
        assigns = []  # List to store assignment elements
        self._collect_assigns(block_stat_node, assigns, 1)  # Collect assignments starting at order 1
        return assigns

    # Collect assignments recursively
    def _collect_assigns(self, node, assigns, order):
        """Build assignment elements from block statements."""
        if not (isinstance(node, Tree) and node.data == "block_stat" and node.children):
            return order  # No more assignments to process

        var_name = node.children[0].value  # Variable name
        expr_node = node.children[1]  # Expression node

        # Create assignment element with order attribute
        assign_elem = ET.Element("assign", {"order": str(order)})
        var_elem = ET.Element("var", {"name": var_name})  # Variable element
        assign_elem.append(var_elem)

        # Process expression and add to assignment
        expr_elem = ET.Element("expr")
        expr_content = self._process_expr(expr_node)
        if expr_content is not None:
            expr_elem.append(expr_content)
        assign_elem.append(expr_elem)

        assigns.append(assign_elem)  # Add to list of assignments

        # Recurse for more assignments if present
        if len(node.children) > 2:
            order = self._collect_assigns(node.children[2], assigns, order + 1)

        return order

    # Unescape string literals
    def _unescape_string(self, raw_string):
        """Remove quotes from string literals."""
        if raw_string.startswith("'") and raw_string.endswith("'"):
            raw_string = raw_string[1:-1]  # Strip surrounding quotes
        return raw_string

    # Process expression nodes into XML
    def _process_expr(self, expr_node):
        """Transform expression into XML representation."""
        if not (isinstance(expr_node, Tree) and expr_node.data == "expr" and expr_node.children):
            return None  # Invalid or empty expression

        expr_base = expr_node.children[0]  # Base of the expression
        expr_tail = expr_node.children[1] if len(expr_node.children) > 1 else None  # Optional tail

        if not (isinstance(expr_base, Tree) and expr_base.data == "expr_base" and expr_base.children):
            return None  # Invalid base

        base_token = expr_base.children[0]  # Token or subtree

        # Handle nested expressions or blocks
        if isinstance(base_token, Tree):
            if base_token.data == "expr":
                return self._process_expr(base_token)  # Recurse for nested expression
            elif base_token.data == "block":
                return self.block(base_token)  # Transform block

        # Process leaf tokens
        if not isinstance(base_token, Tree):
            token_type = base_token.type
            token_value = base_token.value

            # Handle basic literals
            if token_type == "INT":
                return ET.Element("literal", {"class": "Integer", "value": token_value})
            elif token_type == "STR":
                unescaped_value = self._unescape_string(token_value)
                return ET.Element("literal", {"class": "String", "value": unescaped_value})
            elif token_type == "NIL":
                return ET.Element("literal", {"class": "Nil", "value": "nil"})
            elif token_type == "TRUE":
                return ET.Element("literal", {"class": "True", "value": "true"})
            elif token_type == "FALSE":
                return ET.Element("literal", {"class": "False", "value": "false"})
            elif token_type == "ID":
                return ET.Element("var", {"name": token_value})  # Variable reference
            elif token_type == "SELF":
                if expr_tail:  # Handle method call on self
                    if isinstance(expr_tail, Tree) and expr_tail.data == "expr_tail":
                        selector_token = expr_tail.children[0]
                        if not isinstance(selector_token, Tree) and selector_token.type == "ID":
                            send_elem = ET.Element("send", {"selector": selector_token.value})
                            receiver_elem = ET.Element("expr")
                            receiver_elem.append(ET.Element("self"))
                            send_elem.append(receiver_elem)
                            return send_elem
                        elif (isinstance(selector_token, Tree) and
                              selector_token.data == "expr_sel"):
                            send_elem = ET.Element(
                                "send",
                                {"selector": self._build_selector_from_expr_sel(selector_token)}
                            )
                            receiver_elem = ET.Element("expr")
                            receiver_elem.append(ET.Element("self"))
                            send_elem.append(receiver_elem)
                            self._process_expr_sel(selector_token, send_elem)
                            return send_elem
                return ET.Element("self")  # Standalone self
            elif (token_type == "CID" and expr_tail and
                  isinstance(expr_tail, Tree) and expr_tail.data == "expr_tail"):
                selector_token = expr_tail.children[0]
                if not isinstance(selector_token, Tree) and selector_token.type == "ID":
                    send_elem = ET.Element("send", {"selector": selector_token.value})
                    receiver_elem = ET.Element("expr")
                    receiver_content = ET.Element("literal", {"class": "class", "value": token_value})
                    receiver_elem.append(receiver_content)
                    send_elem.append(receiver_elem)
                    return send_elem
                elif (isinstance(selector_token, Tree) and
                      selector_token.data == "expr_sel"):
                    send_elem = ET.Element(
                        "send",
                        {"selector": self._build_selector_from_expr_sel(selector_token)}
                    )
                    receiver_elem = ET.Element("expr")
                    receiver_content = ET.Element("literal", {"class": "class", "value": token_value})
                    receiver_elem.append(receiver_content)
                    send_elem.append(receiver_elem)
                    self._process_expr_sel(selector_token, send_elem)
                    return send_elem
            elif (token_type in ("CID", "ID", "INT", "STR", "NIL", "TRUE", "FALSE", "SELF") and
                  expr_tail and isinstance(expr_tail, Tree) and expr_tail.data == "expr_sel"):
                send_elem = ET.Element(
                    "send",
                    {"selector": self._build_selector_from_expr_sel(expr_tail)}
                )
                receiver_elem = ET.Element("expr")
                # Map token type to XML element
                if token_type == "CID":
                    receiver_content = ET.Element("literal", {"class": "class", "value": token_value})
                elif token_type == "ID":
                    receiver_content = ET.Element("var", {"name": token_value})
                elif token_type == "INT":
                    receiver_content = ET.Element("literal", {"class": "Integer", "value": token_value})
                elif token_type == "STR":
                    receiver_content = ET.Element(
                        "literal",
                        {"class": "String", "value": self._unescape_string(token_value)}
                    )
                elif token_type == "NIL":
                    receiver_content = ET.Element("literal", {"class": "Nil", "value": "nil"})
                elif token_type == "TRUE":
                    receiver_content = ET.Element("literal", {"class": "True", "value": "true"})
                elif token_type == "FALSE":
                    receiver_content = ET.Element("literal", {"class": "False", "value": "false"})
                elif token_type == "SELF":
                    receiver_content = ET.Element("self")
                receiver_elem.append(receiver_content)
                send_elem.append(receiver_elem)
                self._process_expr_sel(expr_tail, send_elem)  # Process arguments
                return send_elem

        return None  # No valid transformation

    # Build selector string from expression selector node
    def _build_selector_from_expr_sel(self, expr_sel_node):
        """Construct selector string from expression selector."""
        if not (isinstance(expr_sel_node, Tree) and
                expr_sel_node.data == "expr_sel" and expr_sel_node.children):
            return ""  # No selector to build

        selector = ""
        sel_token = expr_sel_node.children[0]
        if not isinstance(sel_token, Tree) and sel_token.type == "SEL":
            selector += sel_token.value  # Add keyword part

        # Recurse for additional selector parts
        if (len(expr_sel_node.children) > 2 and
                isinstance(expr_sel_node.children[2], Tree) and
                expr_sel_node.children[2].data == "expr_sel"):
            selector += self._build_selector_from_expr_sel(expr_sel_node.children[2])

        return selector

    # Process expression selector arguments
    def _process_expr_sel(self, expr_sel_node, send_elem, order=1):
        """Append arguments to send element from expression selector."""
        if not (isinstance(expr_sel_node, Tree) and
                expr_sel_node.data == "expr_sel" and expr_sel_node.children):
            return  # No arguments to process

        arg_base = expr_sel_node.children[1]  # Argument base node
        if (isinstance(arg_base, Tree) and arg_base.data == "expr_base" and
                arg_base.children):
            arg_token = arg_base.children[0]
            arg_elem = ET.Element("arg", {"order": str(order)})  # Create argument element
            arg_expr = ET.Element("expr")  # Expression wrapper

            # Process argument content
            if isinstance(arg_token, Tree) and arg_token.data == "expr":
                arg_content = self._process_expr(arg_token)
            elif isinstance(arg_token, Tree) and arg_token.data == "block":
                arg_content = self.block(arg_token)
            elif not isinstance(arg_token, Tree):
                if arg_token.type == "INT":
                    arg_content = ET.Element("literal", {"class": "Integer", "value": arg_token.value})
                elif arg_token.type == "STR":
                    arg_content = ET.Element(
                        "literal",
                        {"class": "String", "value": self._unescape_string(arg_token.value)}
                    )
                elif arg_token.type == "NIL":
                    arg_content = ET.Element("literal", {"class": "Nil", "value": "nil"})
                elif arg_token.type == "TRUE":
                    arg_content = ET.Element("literal", {"class": "True", "value": "true"})
                elif arg_token.type == "FALSE":
                    arg_content = ET.Element("literal", {"class": "False", "value": "false"})
                elif arg_token.type == "ID":
                    arg_content = ET.Element("var", {"name": arg_token.value})
                elif arg_token.type == "CID":
                    arg_content = ET.Element("literal", {"class": "class", "value": arg_token.value})
                else:
                    return  # Unsupported token type
            else:
                return  # Invalid argument type

            # Add argument to send element if valid
            if arg_content is not None:
                arg_expr.append(arg_content)
                arg_elem.append(arg_expr)
                send_elem.append(arg_elem)

        # Recurse for additional arguments
        if (len(expr_sel_node.children) > 2 and
                isinstance(expr_sel_node.children[2], Tree) and
                expr_sel_node.children[2].data == "expr_sel"):
            self._process_expr_sel(expr_sel_node.children[2], send_elem, order + 1)

    # Transform the entire tree into XML
    def transform(self, tree):
        """Convert the syntax tree to an XML structure."""
        self.root = self.program(tree.children)  # Start transformation at program root
        return self.root

# Define the main execution function
def main():
    """Parse input, validate semantics, and output XML."""
    parse_arguments()  # Handle command-line arguments
    try:
        source_code = sys.stdin.read()  # Read input from stdin
    except Exception as e:
        print(f"Error reading input: {e}", file=sys.stderr)
        sys.exit(35)  # Exit with input error code

    # Check for empty input
    if not source_code.strip():
        print("Semantic error: Empty input file", file=sys.stderr)
        sys.exit(31)

    # Extract first comment from source code if present
    comment_match = re.search(r'"([^"]*)"', source_code)
    first_comment = comment_match.group(1) if comment_match else None

    # Initialize parser with SOL25 grammar
    parser = Lark(SOL25_GRAMMAR, start="program", parser="lalr")
    try:
        tree = parser.parse(source_code)  # Parse source into syntax tree
        semantics = SOL25SemanticsVisitor()  # Create semantic validator
        semantics.visit(tree)  # Analyze tree for semantic issues
        semantics.check()  # Perform semantic checks

        # Transform syntax tree to XML
        transformer = SOL25XMLTransformer(first_comment=first_comment)
        xml_tree = transformer.transform(tree)
        ET.indent(xml_tree, space="    ")  # Format XML with indentation
        xml_str = ET.tostring(
            xml_tree,
            encoding="UTF-8",
            method="xml",
            xml_declaration=True
        )
        sys.stdout.buffer.write(xml_str)  # Output XML to stdout
        sys.stdout.write('\n')  # Add newline for cleanliness
    except UnexpectedCharacters as e:
        print(f"Lexical error: {e}", file=sys.stderr)
        sys.exit(21)  # Exit with lexical error code
    except UnexpectedToken as e:
        print(f"Syntactic error: {e}", file=sys.stderr)
        sys.exit(22)  # Exit with syntax error code
    except Exception as e:
        print(f"Other error: {e}", file=sys.stderr)
        sys.exit(99)  # Exit with general error code

# Run the program if executed directly
if __name__ == "__main__":
    main()