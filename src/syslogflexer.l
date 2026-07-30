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

ws [ \t\n\v\f\r]+
number [0-9]+
timestamp [0-9TZ\-:\.]+
single_line_string \"([^\"\\]|\\.)*\"
name [a-zA-Z_][-+a-zA-Z0-9_@]*

nil [\-]
octet [\x01-\xFF]*

%s PRI VERSION TIMESTAMP HOSTNAME APPNAME PROCID MSGID SD SDELEMENT SDPAR MSG
%%
%{
            yylval = yy_value;
%}
<INITIAL>"<" { BEGIN(PRI);
               return '<';}

<PRI>{number} { const auto num = std::stoi(yytext);
                 yylval->build<int>(num);
                 return token::NUMBER;}

<PRI>">" { BEGIN(VERSION);
           return '>';}

<VERSION>{number} {const auto num = std::stoi(yytext);
                   yylval->build<int>(num);
                   return token::NUMBER;}

<VERSION>{ws} { BEGIN TIMESTAMP;
               yylval->build<std::string>(" ");
               return token::SP;}

<TIMESTAMP>{timestamp} { const std::string time = yytext;
                         yylval->build<std::string>(time);
                         return token::ISOTIME;}

<TIMESTAMP>{ws}  { BEGIN HOSTNAME;
                  yylval->build<std::string>(" ");
                  return token::SP;}

<HOSTNAME>{name} { const std::string name = yytext;
                yylval->build<std::string>(name);
                return token::NAME;}

<HOSTNAME>{ws}  { BEGIN APPNAME;
                 yylval->build<std::string>(" ");
                 return token::SP;}

<HOSTNAME>{nil}  { return token::NIL;}

<APPNAME>{name} { const std::string name = yytext;
                   yylval->build<std::string>(name);
                   return token::NAME;}

<APPNAME>{ws}  { BEGIN PROCID;
                 yylval->build<std::string>(" ");
                 return token::SP;}

<APPNAME>{nil}  { return token::NIL;}

<PROCID>{name} { const std::string name = yytext;
                   yylval->build<std::string>(name);
                   return token::NAME;}
<PROCID>{number} { const auto num = std::stoi(yytext);
                   yylval->build<int>(num);
                   return token::NUMBER;}

<PROCID>{ws}  { BEGIN MSGID;
               yylval->build<std::string>(" ");
               return token::SP;}

<PROCID>{nil}  { return token::NIL;}

<MSGID>{name} { const std::string name = yytext;
                yylval->build<std::string>(name);
                return token::NAME;}
<MSGID>{number} { const auto num = std::stoi(yytext);
                   yylval->build<int>(num);
                   return token::NUMBER;}
<MSGID>{ws}  { BEGIN SD;
              yylval->build<std::string>(" ");
              return token::SP;}

<MSGID>{nil}  { return token::NIL;}

<SD>{nil}  { return token::NIL;}

<SD>{ws}    { BEGIN MSG;
             yylval->build<std::string>(" ");
             return token::SP;}

<SD>"["  { BEGIN SDELEMENT;
           return '['; }

<SDELEMENT>"]"  { BEGIN SD;
                  return ']';}

<SDELEMENT>{ws}  { BEGIN SDPAR;
                  yylval->build<std::string>(" ");
                  return token::SP; }

<SDELEMENT>{name}  { const std::string name = yytext;
                     yylval->build<std::string>(name);
                     return token::NAME; }

<SDPAR>"="  { return '='; }

<SDPAR>"]"  { BEGIN SD;
              return ']';}



<SDPAR>{name}  { const std::string name = yytext;
               yylval->build<std::string>(name);
               return token::NAME; }

<SDPAR>{single_line_string}  {
    std::string text = yytext;
    if (text.size() >= 2) {
       text.erase(0,1);
       text.erase(text.size() -1);
    }
    yylval->build<std::string>(text);
    return token::STRING;
    }

<SDPAR>{ws}  {   yylval->build<std::string>(" ");
                return token::SP;}


<MSG>{octet}  { BEGIN INITIAL;
                const std::string name = yytext;
                yylval->build<std::string>(name);
                return token::NAME;}

. {}

%%
