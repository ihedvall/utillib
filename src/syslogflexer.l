%top {
    #include <cstdint>
}
%{

    #include "syslogscanner.h"
    #undef  YY_DECL
    #define YY_DECL int util::syslog::SyslogScanner::yylex(syslog::SyslogParser::value_type* yy_value)
    using token = util::syslog::SyslogParser::token;
%}

%option 8bit
%option nounistd
%option noyywrap
%option debug
%option never-interactive
%option c++
%option yyclass="util::syslog:SyslogScanner"
%option noinput

number [0-9]
timestamp [0-9TZ\-:\.]+
name [!-~]+
id [!#-<>-\\\^-~]+
nil [\-]
value [!#-[\^-\xFF]*
octet [\x01-\xFF]*

%s PRI VERSION TIMESTAMP HOSTNAME APPNAME PROCID MSGID SD SDELEMENT SDPAR SDVALUE MSG
%%
%{
            yylval = yy_value;
%}
<INITIAL>"<" { BEGIN(PRI);
               return '<';}

<PRI>{number}+ { const auto num = std::stoi(yytext);
                 yylval->build<int>(num);
                 return token::NUMBER;}

<PRI>">" { BEGIN(VERSION);
           return '>';}

<VERSION>{number} {const auto num = std::stoi(yytext);
                   yylval->build<int>(num);
                   return token::NUMBER;}

<VERSION>" " { BEGIN TIMESTAMP;
               return token::SP;}

<TIMESTAMP>{timestamp} { const std::string time = yytext;
                         yylval->build<std::string>(time);
                         return token::ISOTIME;}

<TIMESTAMP>" "  { BEGIN HOSTNAME;
                  return token::SP;}

<HOSTNAME>{name} { const std::string name = yytext;
                yylval->build<std::string>(name);
                return token::NAME;}

<HOSTNAME>" "  { BEGIN APPNAME;
                 return token::SP;}

<APPNAME>{name} { const std::string name = yytext;
                   yylval->build<std::string>(name);
                   return token::NAME;}

<APPNAME>" "  { BEGIN PROCID;
                 return token::SP;}

<PROCID>{name} { const std::string name = yytext;
                   yylval->build<std::string>(name);
                   return token::NAME;}

<PROCID>" "  { BEGIN MSGID;
               return token::SP;}

<MSGID>{name} { const std::string name = yytext;
                yylval->build<std::string>(name);
                return token::NAME;}

<MSGID>" "  { BEGIN SD;
              return token::SP;}

<SD>{nil}  { return token::NIL;}

<SD>" "    { BEGIN MSG;
             return token::SP;}

<SD>"["  { BEGIN SDELEMENT;
           return '['; }

<SDELEMENT>"]"  { BEGIN SD;
                  return ']';}

<SDELEMENT>" "  { BEGIN SDPAR;
                  return token::SP; }

<SDELEMENT>{value}  { const std::string name = yytext;
                     yylval->build<std::string>(name);
                     return token::NAME; }


<SDPAR>"="  { return '='; }

<SDPAR>"\""  { BEGIN SDVALUE;
              return '\"'; }

<SDPAR>"]"  { BEGIN SD;
              return ']';}

<SDPAR>" "  { return token::SP;}

<SDPAR>{id}  { const std::string name = yytext;
               yylval->build<std::string>(name);
               return token::NAME; }

<SDVALUE>"\""  { BEGIN SDPAR;
                 return '\"'; }

<SDVALUE>{value}  { const std::string name = yytext;
                   yylval->build<std::string>(name);
                   return token::NAME;}

<MSG>{octet}  { const std::string name = yytext;
                yylval->build<std::string>(name);
                return token::NAME;}

. {}
%%

