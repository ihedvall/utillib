// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "syslogparser.hpp"


// Unqualified %code blocks.
#line 14 "D:/projects/utillib/src/syslogparser.y"

    #include <sstream>
    #include "syslogscanner.h"
    #include "util/logstream.h"
    #undef yylex
    #define yylex scanner.yylex



#line 56 "D:/projects/utillib/src/syslogparser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "D:/projects/utillib/src/syslogparser.y"
namespace util { namespace syslog {
#line 130 "D:/projects/utillib/src/syslogparser.cpp"

  /// Build a parser object.
  SyslogParser::SyslogParser (util::syslog::SyslogScanner &scanner_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg)
  {}

  SyslogParser::~SyslogParser ()
  {}

  SyslogParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  SyslogParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
  {
    switch (this->kind ())
    {
      case symbol_kind::S_NUMBER: // "int"
        value.copy< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  SyslogParser::symbol_kind_type
  SyslogParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  SyslogParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  SyslogParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_NUMBER: // "int"
        value.move< int > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

  }

  // by_kind.
  SyslogParser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  SyslogParser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  SyslogParser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  SyslogParser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  SyslogParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  SyslogParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  SyslogParser::symbol_kind_type
  SyslogParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  SyslogParser::symbol_kind_type
  SyslogParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  SyslogParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  SyslogParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  SyslogParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  SyslogParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  SyslogParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  SyslogParser::symbol_kind_type
  SyslogParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  SyslogParser::stack_symbol_type::stack_symbol_type ()
  {}

  SyslogParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "int"
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  SyslogParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "int"
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  SyslogParser::stack_symbol_type&
  SyslogParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "int"
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  SyslogParser::stack_symbol_type&
  SyslogParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "int"
        value.move< int > (that.value);
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  SyslogParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  SyslogParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  SyslogParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  SyslogParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  SyslogParser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SyslogParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SyslogParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SyslogParser::debug_level_type
  SyslogParser::debug_level () const
  {
    return yydebug_;
  }

  void
  SyslogParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  SyslogParser::state_type
  SyslogParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  SyslogParser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  SyslogParser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SyslogParser::operator() ()
  {
    return parse ();
  }

  int
  SyslogParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_NUMBER: // "int"
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_SP: // "space"
      case symbol_kind::S_ISOTIME: // "isotime"
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_OCTET: // "octet"
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // syslog_message: pri version "space" isotime "space" hostname "space" appname "space" procid "space" msgid "space" sd msg
#line 38 "D:/projects/utillib/src/syslogparser.y"
                                                                               {
    }
#line 690 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 3: // pri: '<' "int" '>'
#line 41 "D:/projects/utillib/src/syslogparser.y"
                     { if (scanner.debug()) std::cout << "PRI: " << yystack_[1].value.as < int > () << std::endl;
        const auto pri = static_cast<uint8_t>(yystack_[1].value.as < int > ());
        const auto severity = static_cast<util::syslog::SyslogSeverity>(pri % 8);
        const auto facility = static_cast<util::syslog::SyslogFacility>(pri / 8);
        scanner.Severity(severity);
        scanner.Facility(facility);
    }
#line 702 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 4: // version: %empty
#line 49 "D:/projects/utillib/src/syslogparser.y"
                { if (scanner.debug()) std::cout << "VERSION: 0" << std::endl;
        scanner.Version(0);
    }
#line 710 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 5: // version: "int"
#line 52 "D:/projects/utillib/src/syslogparser.y"
             { if (scanner.debug()) std::cout << "VERSION: " << yystack_[0].value.as < int > () << std::endl;
        scanner.Version(static_cast<uint8_t>(yystack_[0].value.as < int > ()));
    }
#line 718 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 6: // isotime: "isotime"
#line 56 "D:/projects/utillib/src/syslogparser.y"
                 { if (scanner.debug()) std::cout << "ISOTIME: " << yystack_[0].value.as < std::string > () << std::endl;
        scanner.IsoTime(yystack_[0].value.as < std::string > ());
    }
#line 726 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 8: // hostname: NAME
#line 61 "D:/projects/utillib/src/syslogparser.y"
               { if (scanner.debug()) std::cout << "HOSTNAME: " << yystack_[0].value.as < std::string > () << std::endl;
        scanner.Hostname(yystack_[0].value.as < std::string > ());
    }
#line 734 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 10: // appname: NAME
#line 66 "D:/projects/utillib/src/syslogparser.y"
              { if (scanner.debug()) std::cout << "APPNAME: " << yystack_[0].value.as < std::string > () << std::endl;
        scanner.ApplicationName(yystack_[0].value.as < std::string > ());
     }
#line 742 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 12: // procid: NAME
#line 71 "D:/projects/utillib/src/syslogparser.y"
             { if (scanner.debug()) std::cout << "PID: " << yystack_[0].value.as < std::string > () << std::endl;
        scanner.ProcessId(yystack_[0].value.as < std::string > ());
     }
#line 750 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 13: // procid: "int"
#line 74 "D:/projects/utillib/src/syslogparser.y"
              {scanner.ProcessId(std::to_string(yystack_[0].value.as < int > ()));}
#line 756 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 15: // msgid: NAME
#line 77 "D:/projects/utillib/src/syslogparser.y"
            { if (scanner.debug()) std::cout << "MSG-ID: " << yystack_[0].value.as < std::string > () << std::endl;
        scanner.MessageId(yystack_[0].value.as < std::string > ());
     }
#line 764 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 16: // msgid: "int"
#line 80 "D:/projects/utillib/src/syslogparser.y"
              {scanner.MessageId(std::to_string(yystack_[0].value.as < int > ()));}
#line 770 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 23: // sd_id: NAME
#line 91 "D:/projects/utillib/src/syslogparser.y"
            {
    if (scanner.debug()) std::cout << "ID: " << yystack_[0].value.as < std::string > () << std::endl;
    scanner.AddStructuredData(yystack_[0].value.as < std::string > ());
}
#line 779 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 26: // sd_param: NAME '=' STRING
#line 99 "D:/projects/utillib/src/syslogparser.y"
                          {
     if (scanner.debug()) std::cout << "PAR: " << yystack_[2].value.as < std::string > () << ", VALUE: " << yystack_[0].value.as < std::string > () << std::endl;
     scanner.AppendParameter(yystack_[2].value.as < std::string > (),yystack_[0].value.as < std::string > ());
     }
#line 788 "D:/projects/utillib/src/syslogparser.cpp"
    break;

  case 29: // msg_any: NAME
#line 109 "D:/projects/utillib/src/syslogparser.y"
              { if (scanner.debug()) std::cout << "MSG: " << yystack_[0].value.as < std::string > () << std::endl;
    scanner.Message(yystack_[0].value.as < std::string > ());
    }
#line 796 "D:/projects/utillib/src/syslogparser.cpp"
    break;


#line 800 "D:/projects/utillib/src/syslogparser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  SyslogParser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  SyslogParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0









  const signed char SyslogParser::yypact_ninf_ = -18;

  const signed char SyslogParser::yytable_ninf_ = -1;

  const signed char
  SyslogParser::yypact_[] =
  {
      -9,     5,    17,    15,     9,   -18,   -18,    18,   -18,     6,
     -18,   -18,    19,     7,   -18,   -18,    20,    10,   -18,   -18,
      21,    -2,   -18,   -18,   -18,    22,    -1,   -18,   -18,   -18,
      23,     0,   -18,    24,    25,    26,   -18,   -18,    28,    27,
     -18,   -18,    29,   -18,   -18,    13,    -3,   -18,    30,    29,
     -18,   -18,   -18
  };

  const signed char
  SyslogParser::yydefact_[] =
  {
       0,     0,     0,     4,     0,     1,     5,     0,     3,     0,
       6,     7,     0,     0,     8,     9,     0,     0,    10,    11,
       0,     0,    13,    12,    14,     0,     0,    16,    15,    17,
       0,     0,    18,     0,    27,    19,    20,    23,     0,     0,
       2,    21,     0,    29,    28,     0,     0,    24,     0,     0,
      22,    26,    25
  };

  const signed char
  SyslogParser::yypgoto_[] =
  {
     -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,
     -18,    -6,   -18,   -18,   -17,   -18,   -18
  };

  const signed char
  SyslogParser::yydefgoto_[] =
  {
       0,     2,     3,     7,    12,    16,    20,    25,    30,    34,
      35,    36,    38,    46,    47,    40,    44
  };

  const signed char
  SyslogParser::yytable_[] =
  {
      49,     1,    22,    27,    23,    28,    24,    29,    32,     4,
      50,    10,    33,    14,    11,    15,    18,     5,    19,     6,
       8,     9,    13,    17,    21,    26,    31,    48,    39,    41,
      37,    42,    52,    43,     0,    45,     0,    51,    33
  };

  const signed char
  SyslogParser::yycheck_[] =
  {
       3,    10,     4,     4,     6,     6,     8,     8,     8,     4,
      13,     5,    12,     6,     8,     8,     6,     0,     8,     4,
      11,     3,     3,     3,     3,     3,     3,    14,     3,    35,
       6,     3,    49,     6,    -1,     6,    -1,     7,    12
  };

  const signed char
  SyslogParser::yystos_[] =
  {
       0,    10,    16,    17,     4,     0,     4,    18,    11,     3,
       5,     8,    19,     3,     6,     8,    20,     3,     6,     8,
      21,     3,     4,     6,     8,    22,     3,     4,     6,     8,
      23,     3,     8,    12,    24,    25,    26,     6,    27,     3,
      30,    26,     3,     6,    31,     6,    28,    29,    14,     3,
      13,     7,    29
  };

  const signed char
  SyslogParser::yyr1_[] =
  {
       0,    15,    16,    17,    18,    18,    19,    19,    20,    20,
      21,    21,    22,    22,    22,    23,    23,    23,    24,    24,
      25,    25,    26,    27,    28,    28,    29,    30,    30,    31
  };

  const signed char
  SyslogParser::yyr2_[] =
  {
       0,     2,    15,     3,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     5,     1,     1,     3,     3,     0,     2,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const SyslogParser::yytname_[] =
  {
  "\"end of message\"", "error", "\"invalid token\"", "\"space\"",
  "\"int\"", "\"isotime\"", "NAME", "STRING", "NIL", "\"octet\"", "'<'",
  "'>'", "'['", "']'", "'='", "$accept", "syslog_message", "pri",
  "version", "isotime", "hostname", "appname", "procid", "msgid", "sd",
  "sd_element_list", "sd_element", "sd_id", "sd_param_list", "sd_param",
  "msg", "msg_any", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const signed char
  SyslogParser::yyrline_[] =
  {
       0,    38,    38,    41,    49,    52,    56,    59,    61,    64,
      66,    69,    71,    74,    75,    77,    80,    81,    83,    84,
      86,    87,    89,    91,    96,    97,    99,   106,   107,   109
  };

  void
  SyslogParser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  SyslogParser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  SyslogParser::symbol_kind_type
  SyslogParser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      10,    14,    11,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    12,     2,    13,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9
    };
    // Last valid token kind.
    const int code_max = 264;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 5 "D:/projects/utillib/src/syslogparser.y"
} } // util::syslog
#line 1177 "D:/projects/utillib/src/syslogparser.cpp"

#line 112 "D:/projects/utillib/src/syslogparser.y"



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
