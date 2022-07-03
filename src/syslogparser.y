%skeleton "lalr1.cc"
%require "3.2"
%language "c++"

%define api.namespace {util::syslog}
%define api.parser.class {SyslogParser}
%code requires {
    namespace util::syslog {
        class SyslogScanner;
    }
}
%parse-param { util::syslog::SyslogScanner &scanner  }

%code  {
    #include <sstream>
    #include "syslogscanner.h"
    #undef yylex
    #define yylex scanner.yylex


}

%define api.value.type variant

%token <std::string> SP "space"
%token <int>         NUMBER "int"
%token <std::string> ISOTIME "isotime"
%token <std::string> NAME
%token               NIL
%token <std::string> OCTET "octet"
%token               EOL 0 "end of message"
%start syslog_message
%%

syslog_message:
    pri version SP isotime SP hostname SP appname SP procid SP msgid SP sd msg {
    };

pri : '<' NUMBER '>' { if (scanner.debug()) std::cout << "PRI: " << $2 << std::endl;
        const auto pri = static_cast<uint8_t>($2);
        const auto severity = static_cast<util::syslog::SyslogSeverity>(pri % 8);
        const auto facility = static_cast<util::syslog::SyslogFacility>(pri / 8);
        scanner.Severity(severity);
        scanner.Facility(facility);
    };

version: %empty { if (scanner.debug()) std::cout << "VERSION: 0" << std::endl;
        scanner.Version(0);
    }
    | NUMBER { if (scanner.debug()) std::cout << "VERSION: " << $1 << std::endl;
        scanner.Version(static_cast<uint8_t>($1));
    };

isotime: ISOTIME { if (scanner.debug()) std::cout << "ISOTIME: " << $1 << std::endl;
        scanner.IsoTime($1);
    };

hostname: NAME { if (scanner.debug()) std::cout << "HOSTNAME: " << $1 << std::endl;
        scanner.Hostname($1);
    };

appname: NAME { if (scanner.debug()) std::cout << "APPNAME: " << $1 << std::endl;
        scanner.ApplicationName($1);
     };

procid: NAME { if (scanner.debug()) std::cout << "PID: " << $1 << std::endl;
        scanner.ProcessId($1);
     };

msgid: NAME { if (scanner.debug()) std::cout << "MSG-ID: " << $1 << std::endl;
        scanner.MessageId($1);
     };

sd: NIL
    | sd_element_list;

sd_element_list: sd_element
    | sd_element_list sd_element;

sd_element: '[' sd_id SP sd_param_list ']';

sd_id: NAME { if (scanner.debug()) std::cout << "ID: " << $1 << std::endl;
    scanner.AddStructuredData($1);
};

sd_param_list: %empty
        | sd_param
        | sd_param_list SP sd_param;

sd_param: NAME '=' '\"' NAME '\"' {
     if (scanner.debug()) std::cout << "PAR: " << $1 << ", VALUE: " << $4 << std::endl;
     scanner.AppendParameter($1,$4);
     };

msg : %empty
      | SP msg_any;

msg_any: NAME { if (scanner.debug()) std::cout << "MSG: " << $1 << std::endl;
    scanner.Message($1);
    };
%%


void util::syslog::SyslogParser::error(const std::string& err) {
    std::cout << "Error: " << err << std::endl;
}
