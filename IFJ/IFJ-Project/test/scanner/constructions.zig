const ifj = @import("ifj24.zig");

pub fn main() void {
    // deklarace
    var int1: i32 = 0;
    var real1: f64 = 0.0;
    var char1: []u8 = "abc";
    var boolean1: bool = true;

    // deklarace bez určení typu
    var int2 = 0;
    var real2 = 0.0;
    var char2 = "abc";
    var boolean2 = true;

    // deklarace konstanty
    const int3: i32 = 0;
    const real3: f64 = 0.0;
    const char3: []u8 = "abc";
    const boolean3: bool = true;

    // deklarace konstanty bez určení typu
    const int4 = 0;
    const real4 = 0.0;
    const char4 = "abc";
    const boolean4 = true;

    //deklarace nulovatelných proměnných
    var something = null;
    var int5: ?i32 = null;
    var real5: ?f64 = null;
    var char5: ?[]u8 = null;
    //var boolean5: ?bool = null;

    //prirazeni hodnot
    var int6: i32 = 0;
    int6 = 1;
    var real6: f64 = 0.0;
    real6 = 1.0;
    var char6: []u8 = "abc";
    char6 = "def";
    char6 =
        \\d
        \\e
        \\f
    ;
    char6 = "\", \n, \r, \t, \\";
    var boolean6: bool = true;
    boolean6 = false;

    //prirazeni hodnot konstantam - chyba
    //const int7: i32 = 0;
    //int7 = 1;
    //const real7: f64 = 0.0;
    //real7 = 1.0;
    //const char7: []u8 = "abc";
    //char7 = "def";
    //const boolean7: bool = true;
    //boolean7 = false;

    //prirazeni hodnot nulovatelnym proměnnym
    var int8: ?i32 = null;
    int8 = 1;
    var real8: ?f64 = null;
    real8 = 1.0;
    var char8: ?[]u8 = null;
    char8 = "def";
    //var boolean8: ?bool = null;
    //boolean8 = false;

    //true
    var if1: i32 = 0;
    if (1) {
        if1 = 1;
    } else {
        if1 = 0;
    }

    //false
    var if2: i32 = 0;
    if (0) {
        if2 = 1;
    } else {
        if2 = 0;
    }

    //false
    var if3: i32 = 0;
    if (if3) {
        if3 = 1;
    } else {
        if3 = 0;
    }

    //true
    var if4: i32 = 0;
    if (if4 == 0) {
        if4 = 1;
    } else {
        if4 = 0;
    }

    //false
    var if5: i32 = 0;
    if (if5 == 1) {
        if5 = 1;
    } else {
        if5 = 0;
    }

    //true
    var if6: i32 = 0;
    if (if6 != 1) {
        if6 = 1;
    } else {
        if6 = 0;
    }

    //true
    var if7: i32 = 0;
    if (if7 + 1 == 0 + 1) {
        if7 = 1;
    } else {
        if7 = 0;
    }

    //true
    var if8: i32 = 0;
    if (1.1) {
        if8 = 1;
    } else {
        if8 = 0;
    }

    //false
    var if9: i32 = 0;
    if (0.0) {
        if9 = 1;
    } else {
        if9 = 0;
    }

    //true
    var if10: i32 = 0;
    if ("abc") {
        if10 = 1;
    } else {
        if10 = 0;
    }

    //false
    var if11: i32 = 0;
    if ("") {
        if11 = 1;
    } else {
        if11 = 0;
    }

    //true
    var if12: i32 = 0;
    if (true) {
        if12 = 1;
    } else {
        if12 = 0;
    }

    //false
    var if13: i32 = 0;
    if (false) {
        if13 = 1;
    } else {
        if13 = 0;
    }

    //false
    var if14: i32 = 0;
    if (null) |result| {
        if14 = result;
    } else {
        if14 = 0;
    }

    //false
    var if15: i32 = 0;
    var if16: ?i32 = null;
    if (if16) |result| {
        if15 = result;
    } else {
        if15 = 0;
    }
    //pzn. result ve false vetvi neexistuje!!!

    //true
    var if17: i32 = 0;
    var if18: ?i32 = 1;
    if (if18) |result| {
        if17 = result;
    } else {
        if17 = 0;
    }

    //false
    var if19: f64 = 0;
    var if20: ?f64 = null;
    if (if20) |result| {
        if19 = result;
    } else {
        if19 = 0;
    }

    //true
    var if21: f64 = 0;
    var if22: ?f64 = 1.1;
    if (if22) |result| {
        if21 = result;
    } else {
        if21 = 0;
    }

    //false
    var if23: []u8 = "";
    var if24: ?[]u8 = null;
    if (if24) |result| {
        if23 = result;
    } else {
        if23 = "";
    }

    //true
    var if25: []u8 = "";
    var if26: ?[]u8 = "abc";
    if (if26) |result| {
        if25 = result;
    } else {
        if25 = "";
    }

    //false
    var if27: bool = false;
    var if28: ?bool = null;
    if (if28) |result| {
        if27 = result;
    } else {
        if27 = false;
    }

    //true
    var if29: bool = false;
    var if30: ?bool = true;
    if (if30) |result| {
        if29 = result;
    } else {
        if29 = false;
    }

    //true
    var if31: i32 = 0;
    if (1) {
        if31 = 1;
    }

    //false
    var if32: i32 = 0;
    if (0) {
        if32 = 1;
    }

    //false
    var if33: i32 = 0;
    if (if33) {
        if33 = 1;
    }

    //true
    var if34: i32 = 0;
    if (if34 == 0) {
        if34 = 1;
    }

    //false
    var if35: i32 = 0;
    if (if35 == 1) {
        if35 = 1;
    }

    //true
    var if36: i32 = 0;
    if (if36 != 1) {
        if36 = 1;
    }

    //true
    var if37: i32 = 0;
    if (if37 + 1 == 0 + 1) {
        if37 = 1;
    }

    //true
    var if38: i32 = 0;
    if (1.1) {
        if38 = 1;
    }

    //false
    var if39: i32 = 0;
    if (0.0) {
        if39 = 1;
    }

    //true
    var if40: i32 = 0;
    if ("abc") {
        if40 = 1;
    }

    //false
    var if41: i32 = 0;
    if ("") {
        if41 = 1;
    }

    //true
    var if42: i32 = 0;
    if (true) {
        if42 = 1;
    }

    //false
    var if43: i32 = 0;
    if (false) {
        if43 = 1;
    }

    //true
    var if44: i32 = 0;
    if (null) {
        if44 = 1;
    }

    //false
    var if45: i32 = 0;
    var if46: ?i32 = null;
    if (if46) |result| {
        if45 = result;
    }

    //true
    var if47: i32 = 0;
    var if48: ?i32 = 1;
    if (if48) |result| {
        if47 = result;
    }

    //false
    var if49: f64 = 0;
    var if50: ?f64 = null;
    if (if50) |result| {
        if49 = result;
    }

    //true
    var if51: f64 = 0;
    var if52: ?f64 = 1.1;
    if (if52) |result| {
        if51 = result;
    }

    //false
    var if53: []u8 = "";
    var if54: ?[]u8 = null;
    if (if54) |result| {
        if53 = result;
    }

    //true
    var if55: []u8 = "";
    var if56: ?[]u8 = "abc";
    if (if56) |result| {
        if55 = result;
    }

    //false
    var if57: bool = false;
    var if58: ?bool = null;
    if (if58) |result| {
        if57 = result;
    }

    //true
    var if59: bool = false;
    var if60: ?bool = true;
    if (if60) |result| {
        if59 = result;
    }

    //0 cyklu
    var while1: i32 = 0;
    while (0) {
        while1 = 1;
    }

    //3 cykly
    var while2: i32 = 3;
    while (while2) {
        while2 = while2 - 1;
    }

    //nekonecny cyklus
    var while3: i32 = 0;
    while (1) {
        while3 = 1;
    }

    //0 cyklu
    var while4: i32 = 0;
    while (0) {
        while4 = 1;
    } else {
        while4 = 0;
    }

    //3 cykly
    var while5: i32 = 3;
    while (while5) {
        while5 = while5 - 1;
    } else {
        while5 = 0;
    }

    //nekonecny cyklus
    var while6: i32 = 1;
    while (1) {
        while6 = while6 + 1;
    } else {
        while6 = 0;
    }

    //0 cyklu
    var while7: i32 = 0;
    while (null) |result| {
        while7 = result;
    } else {
        while7 = 0;
    }

    //3 cykly
    var while8: i32 = 3;
    while (while8) |result| {
        while8 = result - 1;
    } else {
        while8 = 0;
    }

    //nekonecny cyklus
    var while9: i32 = 0;
    while (1) |result| {
        while9 = result;
    } else {
        while9 = 0;
    }

    //0 cyklu
    var while10: f64 = 0;
    while (0) {
        while10 = 1.1;
    }

    //3 cykly
    var while11: f64 = 3.3;
    while (while11) {
        while11 = while11 - 1.1;
    }

    //nekonecny cyklus
    var while12: f64 = 0;
    while (1) {
        while12 = 1.1;
    }

    //0 cyklu
    var while13: f64 = 0;
    while (0) {
        while13 = 1.1;
    } else {
        while13 = 0;
    }

    //3 cykly
    var while14: f64 = 3.3;
    while (while14) {
        while14 = while14 - 1.1;
    } else {
        while14 = 0;
    }

    //nekonecny cyklus
    var while15: f64 = 1.1;
    while (1) {
        while15 = while15 + 1.1;
    } else {
        while15 = 0;
    }

    //0 cyklu
    var while16: f64 = 0;
    while (null) |result| {
        while16 = result;
    } else {
        while16 = 0;
    }

    //3 cykly
    var while17: f64 = 3.3;
    while (while17) |result| {
        while17 = result - 1.1;
    } else {
        while17 = 0;
    }

    //nekonecny cyklus
    var while18: f64 = 0;
    while (1) |result| {
        while18 = result;
    } else {
        while18 = 0;
    }

    //0 cyklu
    var while19: []u8 = "";
    while (0) {
        while19 = "abc";
    }

    //1 cyklus
    var while20: []u8 = "abc";
    while (while20) {
        while20 = "";
    }

    //nekonecny cyklus
    var while21: []u8 = "";
    while (1) {
        while21 = "abc";
    }

    //0 cyklu
    var while22: []u8 = "";
    while (0) {
        while22 = "abc";
    } else {
        while22 = "";
    }

    //1 cyklus
    var while23: []u8 = "abc";
    while (while23) {
        while23 = "";
    } else {
        while23 = "abc";
    }

    //nekonecny cyklus
    var while24: []u8 = "";
    while (1) {
        while24 = "abc";
    } else {
        while24 = "";
    }

    //0 cyklu
    var while25: []u8 = "";
    while (null) |result| {
        while25 = result;
    } else {
        while25 = "";
    }

    //1 cyklus
    var while26: []u8 = "abc";
    while (while26) |result| {
        while26 = "";
    } else {
        while26 = "abc";
    }

    //nekonecny cyklus
    var while27: []u8 = "";
    while (1) |result| {
        while27 = result;
    } else {
        while27 = "";
    }

    //0 cyklu
    var while28: bool = false;
    while (0) {
        while28 = true;
    }

    //1 cyklus
    var while29: bool = true;
    while (while29) {
        while29 = false;
    }

    //nekonecny cyklus
    var while30: bool = false;
    while (1) {
        while30 = true;
    }

    //0 cyklu
    var while31: bool = false;
    while (0) {
        while31 = true;
    } else {
        while31 = false;
    }

    //1 cyklus
    var while32: bool = true;
    while (while32) {
        while32 = false;
    } else {
        while32 = true;
    }

    //nekonecny cyklus
    var while33: bool = false;
    while (1) {
        while33 = true;
    } else {
        while33 = false;
    }

    //0 cyklu
    var while34: bool = false;
    while (0) |result| {
        while34 = result;
    } else {
        while34 = false;
    }

    //1 cyklus
    var while35: bool = true;
    while (while35) |result| {
        while35 = result;
    } else {
        while35 = true;
    }

    //nekonecny cyklus
    var while36: bool = false;
    while (1) |result| {
        while36 = result;
    } else {
        while36 = false;
    }

    //vyskok z cyklu
    var while37: i32 = 0;
    while (1) {
        while37 = 1;
        break;
        while37 = 2;
    }

    //vyskok z cyklu
    var while38: i32 = 0;
    while (1) {
        while38 = 1;
        continue;
        while38 = 2;
    } else {
        while38 = 3;
    }

    //vyskok z cyklu
    var while39: i32 = 0;
    while (1) |result| {
        while39 = 1;
        break;
        while39 = 2;
    } else {
        while39 = 3;
    }

    //pokracovani v cyklu
    var while40: i32 = 1;
    while (while40) {
        while40 = 0;
        continue;
        while40 = 2;
    }

    //pokracovani v cyklu
    var while41: i32 = 1;
    while (while41) {
        while41 = 0;
        continue;
        while41 = 2;
    } else {
        while41 = 3;
    }

    //pokracovani v cyklu
    var while42: i32 = 1;
    while (while42) |result| {
        while42 = 0;
        continue;
        while42 = 2;
    } else {
        while42 = 3;
    }

    //0 cyklu
    var for1: []u8 = "";
    var for2: i32 = 0;
    for (for1) |result| {
        const result1: i32 = @as(i32, result);
        for2 = result1;
    }

    //3 cykly
    var for3: []u8 = "abc";
    var for4: i32 = 0;
    for (for3) |result| {
        const result1: i32 = @as(i32, result);
        for4 = for4 + result1;
    }

    //0 cyklu
    var for5: []u8 = "";
    var for6: i32 = 0;
    for (for5) |result| {
        const result1: i32 = @as(i32, result);
        for6 = result1;
    } else {
        for6 = 0;
    }

    //3 cykly
    var for7: []u8 = "abc";
    var for8: i32 = 0;
    for (for7) |result| {
        const result1: i32 = @as(i32, result);
        for8 = for8 + result1;
    } else {
        for8 = 0;
    }

    function1();
    function2(0, 1.1, "abc", true, null, null, null, null,
        \\a
        \\b
        \\c
    , 1 + 1);

    var int9: i32 = function3();
    var real9: f64 = function4();
    var char9: []u8 = function5();
    var boolean9: bool = function6();
    var int10: ?i32 = function7();
    var real10: ?f64 = function8();
    var char10: ?[]u8 = function9();

    //vraci 0
    var rec = recurzion(10);

    var expr1: i32 = 0 + 1;
    var expr2: i32 = 0 - 1;
    var expr3: i32 = 0 * 1;
    var expr4: i32 = 0 / 1;
    var expr5: i32 = 0 or 1;
    var expr6: i32 = 0 and 1;
    var expr7: i32 = 0 == 1;
    var expr8: i32 = 0 != 1;
    var expr9: i32 = 0 < 1;
    var expr10: i32 = 0 <= 1;
    var expr11: i32 = 0 > 1;
    var expr12: i32 = 0 >= 1;
    var expr13: i32 = null orelse 1;
    var expr14: i32 = 1 orelse 0;
    var expr15: i32 = 1 + expr13;
    var expr16: i32 = 1 + function3();
    var expr17: i32 = 1 + (1 + (1));
    var expr18: i32 = 1 + 2 * 3;
    var expr19: i32 = (1 + 2) * 3;
    var expr20: f64 = 1 + 2 / 3;
    var expr21: f64 = (1 + 2) / 3;
    var expr22: i32 = 1 + 2 or 3;
    var expr23: i32 = (1 + 2) or 3;
    var expr24: i32 = 1 + 2 and 3;
    var expr25: i32 = (1 + 2) and 3;
    var expr26: i32 = 1 + 2 == 3;
    var expr27: i32 = (1 + 2) == 3;
    var expr28: i32 = 1 + 2 != 3;
    var expr29: i32 = (1 + 2) != 3;
    var expr30: i32 = 1 + 2 < 3;
    var expr31: i32 = (1 + 2) < 3;
    var expr32: i32 = 1 + 2 <= 3;
    var expr33: i32 = (1 + 2) <= 3;
    var expr34: i32 = 1 + 2 > 3;
    var expr35: i32 = (1 + 2) > 3;
    var expr36: i32 = 1 + 2 >= 3;
    var expr37: i32 = (1 + 2) >= 3;
    var expr38: i32 = 1 + null orelse 3;
    var expr39: ?i32 = null orelse null;
    var expr40: i32 = 1 + 3 orelse 3;
    var expr41: i32 = 1 + 3 and 3;
    var expr42: i32 = 1 * 3 orelse 3;
    var expr43: i32 = 1 * 3 and 3;
    var expr44: i32 = 3 == 3 and (1 != 2) * 1;
    var expr45: i32 = !0;
    var expr46: i32 = !15 * 3;

    //tady popravdě nevim jak to gunguje
    var retype1: i32 = 1.1;
    var retype2: f64 = 1;
    var retype3: i32 = "a";
    var retype4: []u8 = 98;
    var retype5: bool = 1;
    var retype6: i32 = true;
    var retype7: f64 = "a";
    var retype8: f64 = true;

    var builtin1: []u8 = ifj.chr(98);
    var builtin2: i32 = ifj.length("abc");
    var builtin3: i32 = ifj.ord("abc", 2);
    var builtin4: ?[]u8 = ifj.substring("abc", 2, 1);
    var builtin5: []u8 = ifj.concat("a", "bc");
    var builtin6: i32 = ifj.strcmp("a", "abc");
    var builtin7: []u8 = ifj.string("abc");
    var builtin8: i32 = ifj.readi32();
    var builtin9: f64 = ifj.readf64();
    var builtin10: []u8 = ifj.readstr();
    var builtin11: i32 = ifj.f2i(1.1);
    var builtin12: f64 = ifj.i2f(1);
    ifj.write(1);
    ifj.write(1.1);
    ifj.write("abc");
    ifj.write(true);
}

pub fn function1() void {
    var int1: i32 = 0;
}

pub fn function2(int1: i32, real1: f64, char1: []u8, boolean1: bool, int2: ?i32, real2: ?f64, char2: ?[]u8, char3: []u8) void {
    var int1: i32 = 0;
    return;
}

pub fn function3() i32 {
    return 0 + 0;
}

pub fn function4() f64 {
    return 0.0;
}

pub fn function5() []u8 {
    return "abc";
}

pub fn function6() bool {
    return true;
}

pub fn function7() ?i32 {
    return null;
}

pub fn function8() ?f64 {
    return null;
}

pub fn function9() ?[]u8 {
    return null;
}

pub fn recurzion(n: i32) i32 {
    if (n == 0) {
        return 0;
    } else {
        return recurzion(n - 1);
    }
}

pub fn mrtvyKod() void {
    const a = ifj.readi32();
    ifj.write(a);

    if () {
        if () {
        }
    }else {
        
    }
}
