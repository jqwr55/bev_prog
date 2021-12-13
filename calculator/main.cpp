#include <fcntl.h>
#include <common.h>

enum TokenType : u32 {
    TOKEN_ERROR,

    TOKEN_IDENTIFIER,
    TOKEN_F64,

    TOKEN_ADD,              // +
    TOKEN_SUB,              // -
    TOKEN_MUL,              // *
    TOKEN_DIV,              // /
    TOKEN_MOD,              // %

    TOKEN_OPEN_PAREN,       // (,
    TOKEN_CLOSE_PAREN,      // )
    TOKEN_SEMICOLON,        // ;
    TOkEN_EQUAL,            // =

    TOKEN_KEYWORD_EXIT,     // exit
    TOKEN_KEYWORD_CLEAR,    // clear
    TOKEN_EOF,

    TOKEN_COUNT,
};
const char* TOKEN_STRS[] = {
    "TOKEN_ERROR",
    "TOKEN_F64",
    "TOKEN_ADD",
    "TOKEN_SUB",
    "TOKEN_MUL",
    "TOKEN_DIV",
    "TOKEN_MOD",
    "TOKEN_OPEN_PAREN",
    "TOKEN_CLOSE_PAREN",
    "TOKEN_SEMICOLON",
    "TOkEN_EQUAL",
    "TOKEN_KEYWORD_EXIT",
    "TOKEN_KEYWORD_CLEAR",
    "TOKEN_EOF",
    "TOKEN_COUNT",
};

struct Token {
    TokenType type;
    u32 begin;
    u32 size;
};
enum ExprType : u32 {
    EXPR_NONE,

    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_SYMBOL,
    EXPR_ASSIGNMENT,
    EXPR_CLEAR,

    EXPR_COUNT,
};

struct Expr {
    u32 index;
};
struct LiteralExpr : Expr {
    f64 c;
};
struct BinaryExpr : Expr {
    u32 opr;
    Expr left;
    Expr right;
};
struct SymbolExpr : Expr {
    Token symbol;
};

struct Symbol {
    Token name;
    f64 value;
};
struct Tokenizer {
    const char* begin;
    const char* end;
    const char* at;
};
struct Context {
    const char* name;
    Tokenizer tokenizer;
    Token* tokenStream;
    byte* exprBase;
    Symbol* environment;
    u32 environmentSize;
    u32 environmentCap;
};
enum ContextStatus : u32 {
    CTX_EXIT,
    CTX_EOF,
};
bool IsNumeric(char c) {
    return c >= '0' && c <= '9';
}
bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool TokenEquals(const char* begin, Token t, const char* match) {
    
    auto ptr = begin + t.begin;
    auto end = ptr + t.size;
    for(;;) {
        if(!*match) return ptr == end;
        if(ptr == end) return *match == 0;
        if(*ptr++ != *match++) return false;
    }
}
bool TokensEqual(const char* text, Token t0, Token t1) {

    if(t0.size != t1.size) return false;

    const char* begin0 = text + t0.begin;
    const char* begin1 = text + t1.begin;

    for(u32 i = 0; i < t0.size; i++) {
        if(begin0[i] != begin1[i]) return false;
    }
    return true;
}
struct TokenKeyWord {
    TokenType type;
    const char* name;
};

constexpr TokenKeyWord keyWords[] = {
    {TOKEN_KEYWORD_EXIT,       "exit"},
    {TOKEN_KEYWORD_CLEAR,      "clear"},
};
Token GetToken(Tokenizer* tokenizer) {

    auto it = tokenizer->at;

    //skip white space
    if(*it == '#') {
        while(*it && *it != '\n') it++;
    }
    while(*it != 0 && (*it == ' ' || *it == '\n')) it++;


    Token token;
    token.begin = it - tokenizer->begin;

    switch(*it) {
    case 0:
        token.type = TOKEN_EOF;
        break;
    case ';':
        token.type = TOKEN_SEMICOLON;
        break;
    case '(':
        token.type = TOKEN_OPEN_PAREN;
        break;
    case ')':
        token.type = TOKEN_CLOSE_PAREN;
        break;
    case '+':
        token.type = TOKEN_ADD;
        break;
    case '-':
        token.type = TOKEN_SUB;
        break;
    case '*':
        token.type = TOKEN_MUL;
        break;
    case '/':
        token.type = TOKEN_DIV;
        break;
    case '%':
        token.type = TOKEN_MOD;
        break;
    case '=':
        token.type = TOkEN_EQUAL;
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        {
            u32 pointCount = 0;
            while((IsNumeric(*it) || *it == '.') && *it) pointCount += *it++ == '.';

            if(pointCount < 2 && (IsNumeric( it[-1] ) || it[-1] == '.') ) {
                token.type = TOKEN_F64;
                it--;
            }
            else {
                token.type = TOKEN_ERROR;
            }
        }

        break;
    default:
        if(IsAlpha(*it)) {
            while (IsAlpha(*it) || IsNumeric(*it) || *it == '_') it++;
            token.size = (it - tokenizer->begin) - token.begin;

            token.type = TOKEN_IDENTIFIER;
            for(auto i = keyWords; i < 1[&keyWords]; i++) {
                if(TokenEquals(tokenizer->begin, token, i->name)) {
                    token.type = i->type;
                }
            }

            tokenizer->at = it;
            return token;
        }
        else {
            token.type = TOKEN_ERROR;
        }
        break;
    }
    
    tokenizer->at = ++it;
    token.size = (it - tokenizer->begin) - token.begin;
    return token;
}

u32 Tokenize(Tokenizer* tokenizer, Token* result) {

    u32 count = 0;

    for(auto t = GetToken(tokenizer); t.type != TOKEN_EOF; t = GetToken(tokenizer)) {
        result[count++] = t;
    }
    return count;
}

struct ParsedResult {
    Expr expr;
    u32 consumedTokenCount;
    u32 consumedMemory;
};
u32 Match(Token* token, TokenType* types, u32 count) {
    for(u32 i = 0; i < count; i++) {
        if(token->type == types[i]) return i;
    }

    return ~u32(0);
}
i64 GetI64(Tokenizer* tokenizer, Token t) {
    
    i64 ret = 0;
    auto begin = tokenizer->begin + t.begin;
    i64 neg = (*begin == '-');
    begin += neg;

    for(u32 i = 0; i < t.size; i++) {
        ret *= 10;
        ret += begin[i] - '0';
    }


    ret *= ((!neg << 1) - 1);// conditionally negate
    return ret;
}
f64 GetF64(Tokenizer* tokenizer, Token t) {

    auto begin = tokenizer->begin + t.begin;
    char str[t.size+1];
    str[t.size] = 0;
    memcpy(str, begin, t.size);
    return str_to_f64(str);
}

ParsedResult ParseAssignment(Token* stream, Tokenizer* tokenizer, byte* base, byte* result);
ParsedResult ParsePrimary(Token* stream, Tokenizer* tokenizer, byte* base, byte* result) {

    ParsedResult ret{};

    TokenType tokens[] = {
        TOKEN_IDENTIFIER,
        TOKEN_F64,
        TOKEN_OPEN_PAREN,
    };

    auto opr = Match(stream, tokens, 3);
    switch(opr) {
    case 0:// [identifier]
        {
            ret.expr.index = result - base;
            auto& symbol = Mem<SymbolExpr>(result);
            symbol.index = EXPR_SYMBOL;
            symbol.symbol = *stream;
            
            ret.consumedMemory = sizeof(SymbolExpr);
            ret.consumedTokenCount = 1;
            return ret;
        }
    case 1:// -> f64 literal
        {

            auto& literal = Mem<LiteralExpr>(result);
            literal.index = EXPR_LITERAL;
            literal.c = GetF64(tokenizer, *stream);

            ret.expr.index = result - base;
            ret.consumedMemory = sizeof(LiteralExpr);
            ret.consumedTokenCount = 1;

            return ret;
        }
    case 2:// -> (exp)
        {
            auto groupResult = ParseAssignment(stream+1, tokenizer, base, result);
            groupResult.consumedTokenCount += 2;
            return groupResult;
        }
    default:
        {
            global_print("%i", stream->type);
            global_io_flush();
        }
        ASSERT(false);
        break;
    }

    return ret;
}
ParsedResult ParseFactor(Token* stream, Tokenizer* tokenizer, byte* base, byte* result) {

    auto left = ParsePrimary(stream, tokenizer, base, result);

    TokenType binaries[] = {
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_MOD,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    for(u32 opr = Match(stream, binaries, 3); opr != ~u32(0); opr = Match(stream, binaries, 3)) {

        auto e = Expr{result - base};
        auto& bin = Mem<BinaryExpr>(result);

        bin.index = EXPR_BINARY;
        bin.left = left.expr;
        bin.opr = binaries[opr];

        stream++;
        result += sizeof(BinaryExpr);
        auto right = ParsePrimary(stream, tokenizer, base, result);

        bin.right = right.expr;
        stream += right.consumedTokenCount;
        result += right.consumedMemory;

        left.consumedMemory += right.consumedMemory + sizeof(BinaryExpr);
        left.consumedTokenCount += right.consumedTokenCount + 1;
        left.expr = e;
    }

    return left;
}

ParsedResult ParseTerm(Token* stream, Tokenizer* tokenizer, byte* base, byte* result) {

    auto left = ParseFactor(stream, tokenizer, base, result);

    TokenType binaries[] = {
        TOKEN_ADD,
        TOKEN_SUB,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    for(u32 opr = Match(stream, binaries, 2); opr != ~u32(0); opr = Match(stream, binaries, 2)) {

        auto e = Expr{result - base};
        auto& bin = Mem<BinaryExpr>(result);

        bin.index = EXPR_BINARY;
        bin.left = left.expr;
        bin.opr = binaries[opr];

        stream++;
        result += sizeof(BinaryExpr);
        auto right = ParseFactor(stream, tokenizer, base, result);

        bin.right = right.expr;
        stream += right.consumedTokenCount;
        result += right.consumedMemory;

        left.consumedMemory += right.consumedMemory + sizeof(BinaryExpr);
        left.consumedTokenCount += right.consumedTokenCount + 1;
        left.expr = e;
    }

    return left;
}
ParsedResult ParseAssignment(Token* stream, Tokenizer* tokenizer, byte* base, byte* result) {

    auto left = ParseTerm(stream, tokenizer, base, result);

    TokenType binaries[] = {
        TOkEN_EQUAL,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    for(u32 opr = Match(stream, binaries, 1); opr != ~u32(0); opr = Match(stream, binaries, 1)) {

        auto e = Expr{result - base};
        auto& bin = Mem<BinaryExpr>(result);

        bin.index = EXPR_ASSIGNMENT;
        bin.left = left.expr;
        bin.opr = binaries[opr];

        stream++;
        result += sizeof(BinaryExpr);
        auto right = ParseTerm(stream, tokenizer, base, result);

        bin.right = right.expr;
        stream += right.consumedTokenCount;
        result += right.consumedMemory;

        left.consumedMemory += right.consumedMemory + sizeof(BinaryExpr);
        left.consumedTokenCount += right.consumedTokenCount + 1;
        left.expr = e;
    }

    return left;
}

bool ExpectToken(Token* stream, TokenType t) {
    auto cond = stream->type != t;
    if(cond) {
        global_print("%s%s%c", "Expected ", TOKEN_STRS[t], '\n');
    }
    return !cond;
}

u32 ParseTokenStream(Tokenizer* tokenizer, Token* stream, u32 tokenCount, byte* base, byte* result) {

    // max 100 expressions
    Expr* exprBuffer = (Expr*)result;
    result += sizeof(Expr) * 100;
    u32 count = 0;

    for(u32 i = 0; i < tokenCount;) {

        if(stream[i].type == TOKEN_KEYWORD_CLEAR) {
            Mem<ExprType>(result) = EXPR_CLEAR;
            i++;
            exprBuffer[count++].index = result - base;
            result += sizeof(ExprType);
            continue;
        }
        else if(stream[i].type == TOKEN_KEYWORD_EXIT) {

            Mem<ExprType>(result) = EXPR_NONE;
            i++;
            exprBuffer[count++].index = result - base;
            result += sizeof(ExprType);
            continue;
        }

        auto res = ParseAssignment(stream + i, tokenizer, base, result);
        i += res.consumedTokenCount;
        result += res.consumedMemory;
        exprBuffer[count++] = res.expr;

        i += ExpectToken(stream + i, TOKEN_SEMICOLON);
    }

    return count;
}

f64 EvalOpr(byte* base, u32 operation, f64 left, f64 right) {
    
    switch(operation) {
    case TOKEN_ADD:
        return left + right;
    case TOKEN_SUB:
        return left - right;
    case TOKEN_MUL:
        return left * right;
    case TOKEN_DIV:
        return left / right;
    case TOKEN_MOD:
        {
            f64 div = left / right;
            return left - ((i64)div * right);
        }
    }
    ASSERT(false);
}

Token GetNameExpr(Context* ctx, Expr e) {

    auto exp = (Expr*)(ctx->exprBase + e.index);
    switch(exp->index) {
    case EXPR_NONE: 
    case EXPR_LITERAL:
    case EXPR_BINARY:
        ASSERT(false);
        break;
    case EXPR_ASSIGNMENT:
        {
            auto& assignment = Mem<BinaryExpr>(exp);
            return GetNameExpr(ctx, assignment.left);
            break;
        }
    case EXPR_SYMBOL:
        return Mem<SymbolExpr>(exp).symbol;
    }
    ASSERT(false);
}
f64 EvalExpr(Context* ctx, Expr e) {

    auto exp = (Expr*)(ctx->exprBase + e.index);
    
    switch(exp->index) {
    case EXPR_NONE: ASSERT(false);
    case EXPR_LITERAL:
        return Mem<LiteralExpr>(exp).c;
    case EXPR_BINARY:
        {
            auto& bin = Mem<BinaryExpr>(exp);
            f64 l = EvalExpr(ctx, bin.left);
            f64 r = EvalExpr(ctx, bin.right);
            return EvalOpr(ctx->exprBase, bin.opr, l,r);
        }
    case EXPR_ASSIGNMENT:
        {
            auto& assignment = Mem<BinaryExpr>(exp);
            auto name = GetNameExpr(ctx, assignment.left);
            auto right = EvalExpr(ctx, assignment.right);
            for(u32 i = 0; i < ctx->environmentSize; i++) {
                if(TokensEqual(ctx->tokenizer.begin, ctx->environment[i].name, name)) {
                    ctx->environment[i].value = right;
                    return right;
                }
            }

            u32 index = ctx->environmentSize++;
            ASSERT(index < ctx->environmentCap);
            ctx->environment[index].name = name;
            ctx->environment[index].value = right;
            return right;
        }
    case EXPR_SYMBOL:
        {
            auto& s = Mem<SymbolExpr>(exp);

            for(u32 i = 0; i < ctx->environmentSize; i++) {
                if(TokensEqual(ctx->tokenizer.begin, ctx->environment[i].name, s.symbol)) {
                    return ctx->environment[i].value;
                }
            }
            u32 index = ctx->environmentSize++;
            ASSERT(index < ctx->environmentCap);
            ctx->environment[index].name = s.symbol;
            return ctx->environment[index].value;
        }
    }
    ASSERT(false);
}
void PrintExpr(const char* text, byte* base, Expr e) {

    auto type = Mem<ExprType>(base + e.index);
    switch (type) {
    case EXPR_NONE:
        ASSERT(false);
        break;
    case EXPR_LITERAL:
        global_print("%f", Mem<LiteralExpr>(base + e.index).c);
        break;
    case EXPR_BINARY:
        {
            auto& bin = Mem<BinaryExpr>(base + e.index);
            global_print("%c", '(');
            PrintExpr(text, base, bin.left);
            global_print("%c%s%c", ' ', TOKEN_STRS[bin.opr], ' ');
            PrintExpr(text, base, bin.right);
            global_print("%c", ')');
            break;
        }
    case EXPR_SYMBOL:
        {
            auto& s = Mem<SymbolExpr>(base + e.index);
            auto str = text + s.symbol.begin;
            global_print("%s*", str, s.symbol.size);
            break;
        }
    }
}

Context MakeReplContext(byte* mem) {
    
    Context ctx;
    ctx.name = "repl";
    ctx.tokenizer.begin = (char*)mem;
    ctx.tokenizer.end = (char*)mem + Kilobyte(8);
    ctx.tokenStream = (Token*)(mem + Kilobyte(8));
    ctx.exprBase = mem + Kilobyte(16);
    ctx.environment = (Symbol*)(ctx.exprBase + Kilobyte(8));
    ctx.environmentCap = 150;
    ctx.environmentSize = 0;
    byte* exprMem = ctx.exprBase;
    ctx.tokenizer.at = ctx.tokenizer.begin;
    
    return ctx;
}
u32 ParseArgs(u32 argc, const char* argv[], Context* result, byte* mem) {

    u32 ret = 0;
    for(u32 i = 1; i < argc; i++) {
        auto path = argv[i];
        if(str_cmp(path, "-help")) {

            global_print("s", "To run in repl mode type -repl\n");
            global_print("s", "To run a file type the relative path to the file ex: ./test\n");
            global_io_flush();
            continue;
        }
        if(str_cmp(path, "-repl")) {
            Mem<Context>(result + ret++) = MakeReplContext(mem);
            continue;
        }
        u32 descriptor = open(path, O_RDONLY);
        if(descriptor == ~u32(0)) {
            global_print("%s%s%c", "Errors opening file: ", path, '\n');
            continue;
        }

        auto& ctx = Mem<Context>(result + ret++);
        ctx.name = path;
        ctx.tokenizer.begin = (char*)mem;
        ctx.tokenizer.end = (char*)mem + Kilobyte(8);
        ctx.tokenStream = (Token*)(mem + Kilobyte(8));
        ctx.exprBase = mem + Kilobyte(16);
        ctx.environment = (Symbol*)(ctx.exprBase + Kilobyte(8));
        ctx.environmentCap = 150;
        ctx.environmentSize = 0;
        byte* exprMem = ctx.exprBase;
        ctx.tokenizer.at = ctx.tokenizer.begin;
        auto readBytes = read(descriptor, (void*)ctx.tokenizer.at, Kilobyte(8)-1);
        Mem<char>((char*)(ctx.tokenizer.at + readBytes)) = 0;

        mem += Kilobyte(27) + 600;
    }
    return ret;
}
void RunReplContext(Context* ctx) {

    global_print("%s", "repl mode\ntype \"exit\" to leave");

    bool running = true;
    for(;running;) {

        global_print("%s", "\n>");
        global_io_flush();
        auto readByteCount = read(STDIN_FILENO, (void*)(ctx->tokenizer.at), Kilobyte(8));
        ((char*)(ctx->tokenizer.at))[readByteCount] = 0;

        auto count = Tokenize(&ctx->tokenizer, ctx->tokenStream);
        auto exprCount = ParseTokenStream(&ctx->tokenizer, ctx->tokenStream, count, ctx->exprBase, ctx->exprBase);
        ctx->tokenStream += count;

        for(u32 i = 0; i < exprCount; i++) {
            auto e = ((Expr*)ctx->exprBase)[i];
            auto t = Mem<ExprType>(ctx->exprBase + e.index);
            if(t == EXPR_CLEAR) {
                system("clear");
                continue;
            }
            else if(t == EXPR_NONE) {
                running = false;
                break;
            }
            auto v = EvalExpr(ctx, e);
            global_print("%f%s", v, ", ");
        }
    }
}

ContextStatus RunContext(Context ctx) {

    global_print("%s%s%c", "Running ", ctx.name, '\n');
    auto count = Tokenize(&ctx.tokenizer, ctx.tokenStream);
    auto exprCount = ParseTokenStream(&ctx.tokenizer, ctx.tokenStream, count, ctx.exprBase, ctx.exprBase);

    for(u32 i = 0; i < exprCount; i++) {
        auto e = ((Expr*)ctx.exprBase)[i];
        auto t = Mem<ExprType>(ctx.exprBase + e.index);
        if(t == EXPR_CLEAR) {
            system("clear");
            continue;
        }
        else if(t == EXPR_NONE) {
            return CTX_EXIT;
        }
        auto v = EvalExpr(&ctx, e);
        global_print("%f%s", v, ", ");
    }

    return CTX_EOF;
}

i32 main(u32 argc, const char* argv[]) {

    auto scratch = init_global_state(0, argc * (Kilobyte(27) + 600), 512);

    {
        Context ctxs[argc-1];
        u32 ctxCount = ParseArgs(argc, argv, ctxs, scratch);
        for(u32 i = 0; i < ctxCount; i++) {

            if(str_cmp(ctxs[i].name, "repl")) {
                RunReplContext(ctxs + i);
            }
            else {
                auto res = RunContext(ctxs[i]);
                global_print("%s%s", "\ncontext ", res == CTX_EOF ? "eof\n" : "exited\n");
            }
            global_print("%c", '\n');
        }
    }

    global_io_flush();

    return 0;
}