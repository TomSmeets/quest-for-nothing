#include "os_generic.h"
#include "fmt.h"

struct Parse {
    u8 *buf;
    mem *m;
    bool error;
};

static u8 parse_char(Parse *p) {
    u8 c = *p->buf;
    if(p->error || c == 0) return 0;
    p->buf++;
    return c;
}

static u8 parse_peek(Parse *p) {
    if(p->error) return 0;
    return *p->buf;
}

static Parse parse_new(u8 *str, mem *m) { return (Parse) { .buf = str, .m = m }; }
static void parse_fail(Parse *p) { p->error = 1; }
static Parse parse_save(Parse *p) { return *p; }
static void parse_restore(Parse *p, Parse prev) { *p = prev; }

static u8 parse_digit(Parse *p) {
    u8 d = parse_char(p);
    if (d >= '0' && d <= '9') return d - '0';
    p->error = 1;
    return 0;
}


static void parse_char_matching(Parse *p, char s) {
    u8 c = parse_char(p);
    if(c != s) parse_fail(p);
}

static void parse_symbol(Parse *p, char *sym) {
    while(!p->error && *sym) {
        parse_char_matching(p, *sym++);
    }
}


static u32 parse_u32(Parse *p) {
    u32 n = parse_digit(p);
    if(p->error) return 0;

    for(;;) {
        Parse prev = parse_save(p);
        u32 digit = parse_digit(p);
        if(p->error) {
            parse_restore(p, prev);
            return n;
        }
        n = n * 10 + digit;
    }
}

static void parse_space(Parse *p) {
    parse_char_matching(p, ' ');
    while(parse_peek(p) == ' ') parse_char(p);
}

static bool is_word_char(u8 c) {
    if (c >= 'a' && c <= 'z') return 1;
    if (c >= 'A' && c <= 'Z') return 1;
    if (c == '_') return 1;
    if (c == '.') return 1;
    if (c == '-') return 1;
    return 0;
}

static char *parse_word(Parse *p) {
    u8 *start = p->buf;
    if(!is_word_char(parse_peek(p))) parse_fail(p);
    while(is_word_char(parse_peek(p))) parse_char(p);
    u8 *end = p->buf;
    return str_dup_len(p->m, (char *) start, end - start);
}

static char *parse_til_next_line(Parse *p) {
    if(p->error) return 0;

    u8 *start = p->buf;
    for(;;) {
        char c = parse_char(p);
        if(c == '\n') break;
        if(c == 0) break;
    }
    u8 *end = p->buf-1;
    return str_dup_len(p->m, (char *) start, end - start);
}


struct Balance {
    mem *m;
    Balance_Item *first;
    Balance_Item *last;
};

struct Balance_Item {
    u32 year;
    char *name;
    i32 amount;
    Balance_Item *next;
};



static Balance_Item *balance_get(Balance *b, char *name, u32 year) {
    for(Balance_Item *item = b->first; item; item = item->next){
        if(str_eq(item->name, name) && item->year == year) return item;
    }

    Balance_Item *item = mem_struct(b->m, Balance_Item);
    item->name = str_dup(b->m, name);
    item->year = year;
    if(b->last) {
        b->last->next = item;
        b->last = item;
    } else {
        b->first = b->last = item;
    }
    return item;
}

static void parse_row(Parse *p, Balance *balance) {
    u32 dd = parse_u32(p);
    parse_symbol(p, "-");
    u32 mm = parse_u32(p);
    parse_symbol(p, "-");
    u32 yy = parse_u32(p);

    parse_space(p);

    u32 euro = parse_u32(p);

    // Cents (optional)
    Parse prev = parse_save(p);
    parse_symbol(p, ".");
    u32 cents = 10*parse_digit(p) + parse_digit(p);
    if(p->error) {
        parse_restore(p, prev);
        cents = 0;
    }

    parse_space(p);
    char *from = parse_word(p);
    parse_space(p);
    char *to   = parse_word(p);
    char *rest = parse_til_next_line(p);

    if(p->error) {
        return;
    }
    os_printf("%02d/%02d/%02d %6d.%02d %16s -> %16s %s\n", dd, mm, yy, euro, cents, from, to, rest);
    u32 amount = euro*100 + cents;

    if(str_eq(from, "balance")) {
        balance_get(balance, to, yy)->amount = amount;
        return;
    }

    balance_get(balance, from, yy)->amount -= amount;
    balance_get(balance,   to, yy)->amount += amount;
}

void *main_init(int argc, char **argv) {
    if(argc < 2) {
        os_print("ledger [file]\n");
        os_exit(1);
    }
    mem *m = mem_new();
    char *file_path = argv[1];
    Parse p = parse_new((u8 *) os_read_file(m, file_path).ptr, m);
    Balance b = {};
    b.m = m;
    while(!p.error) {
        parse_row(&p, &b);
    }

    os_print("== Balance ==\n");
    for(Balance_Item *i = b.first; i; i = i->next) {
        bool pos = i->amount >= 0;
        i32 amount = pos ? i->amount : -i->amount;
        os_printf("%4d %16s %6i.%02i\n", i->year, i->name, amount / 100, amount % 100);
    }

    mem_free(m);
    return 0;
}

void main_update(void *handle) { os_exit(0); }
