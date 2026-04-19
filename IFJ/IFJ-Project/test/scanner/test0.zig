const ifj = @import("ifj24.zig");

pub fn bar(param: []u8) []u8 {
    a = 1 + 1;
    const r = foo(param);
    p(null, null, null, null, 3.00);
    p.p(a);
    return r;
}

pub fn foo(par: []u8) []u8 {
    const ret = bar(par);
    p(a);
    return ret;
}

pub fn main() void {
    const par = ifj.string(a, 51, 66);
    _ = bar(par);
}
