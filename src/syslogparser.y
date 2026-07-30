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
    #include "util/logstream.h"
    #undef yylex
    #define yylex scanner.yylex


}

%define api.value.type variant

%token <std::string> SP "space"
%token <int>         NUMBER "int"
%token <std::string> ISOTIME "isotime"
%token <std::string> NAME
%token <std::string> STRING
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
    }
    | NIL;

hostname: NAME { if (scanner.debug()) std::cout << "HOSTNAME: " << $1 << std::endl;
        scanner.Hostname($1);
    }
    | NIL;

appname: NAME { if (scanner.debug()) std::cout << "APPNAME: " << $1 << std::endl;
        scanner.ApplicationName($1);
     }
     | NIL;

procid: NAME { if (scanner.debug()) std::cout << "PID: " << $1 << std::endl;
        scanner.ProcessId($1);
     }
     | NUMBER {scanner.ProcessId(std::to_string($1));}
     | NIL;

msgid: NAME { if (scanner.debug()) std::cout << "MSG-ID: " << $1 << std::endl;
        scanner.MessageId($1);
     }
     | NUMBER {scanner.MessageId(std::to_string($1));}
     | NIL;

sd: NIL
    | sd_element_list;

sd_element_list: sd_element
    | sd_element_list sd_element

sd_element: '[' sd_id SP sd_param_list ']';

sd_id: NAME {
    if (scanner.debug()) std::cout << "ID: " << $1 << std::endl;
    scanner.AddStructuredData($1);
};

sd_param_list: sd_param
	| sd_param_list SP sd_param;

sd_param: NAME '=' STRING {
     if (scanner.debug()) std::cout << "PAR: " << $1 << ", VALUE: " << $3 << std::endl;
     scanner.AppendParameter($1,$3);
     };



msg : %empty
      | SP msg_any;

msg_any: NAME { if (scanner.debug()) std::cout << "MSG: " << $1 << std::endl;
    scanner.Message($1);
    };
%%


void util::syslog::SyslogParser::error(const std::string& err) {
    const auto line = scanner.lineno();
    const std::string near = scanner.YYText() != nullptr ? scanner.YYText() : "";
    std::ostringstream error;
    if (scanner.debug()) {
    	std::cout << "Parser error: " << err
          << ", Line: " << line
          << ", Near: " << near
          << ", Input: " << scanner.Origin();
    }

}