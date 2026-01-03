// clang-format off

#line 1 "emoji_presentation_scanner.rl"
/* Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdbool.h>

#ifndef EMOJI_LINKAGE
#define EMOJI_LINKAGE static
#endif


#line 23 "emoji_presentation_scanner.c"
static const unsigned char _emoji_presentation_trans_keys[] = {
	0u, 13u, 14u, 15u, 0u, 13u, 9u, 12u, 10u, 12u, 10u, 10u, 4u, 12u, 4u, 12u, 
	6u, 6u, 9u, 12u, 8u, 8u, 8u, 10u, 9u, 14u, 0
};

static const char _emoji_presentation_key_spans[] = {
	14, 2, 14, 4, 3, 1, 9, 9, 
	1, 4, 1, 3, 6
};

static const char _emoji_presentation_index_offsets[] = {
	0, 15, 18, 33, 38, 42, 44, 54, 
	64, 66, 71, 73, 77
};

static const char _emoji_presentation_indicies[] = {
	1, 1, 1, 2, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 0, 4, 
	5, 3, 6, 6, 7, 8, 9, 9, 
	10, 11, 9, 9, 9, 9, 9, 12, 
	9, 5, 13, 14, 15, 0, 13, 16, 
	17, 16, 13, 0, 17, 16, 16, 16, 
	16, 16, 13, 16, 17, 16, 17, 16, 
	16, 16, 16, 5, 13, 14, 15, 16, 
	5, 18, 5, 13, 19, 20, 18, 14, 
	21, 23, 22, 13, 22, 5, 13, 14, 
	15, 16, 4, 16, 0
};

static const char _emoji_presentation_trans_targs[] = {
	2, 4, 6, 2, 1, 2, 3, 3, 
	7, 2, 8, 9, 12, 0, 2, 5, 
	2, 5, 2, 10, 11, 2, 2, 2
};

static const char _emoji_presentation_trans_actions[] = {
	1, 2, 2, 3, 0, 4, 7, 2, 
	2, 8, 0, 7, 2, 0, 9, 10, 
	11, 2, 12, 0, 10, 13, 14, 15
};

static const char _emoji_presentation_to_state_actions[] = {
	0, 0, 5, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _emoji_presentation_from_state_actions[] = {
	0, 0, 6, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _emoji_presentation_eof_trans[] = {
	1, 4, 0, 1, 17, 1, 17, 17, 
	19, 19, 22, 23, 17
};

static const int emoji_presentation_start = 2;

static const int emoji_presentation_en_text_and_emoji_run = 2;


#line 26 "emoji_presentation_scanner.rl"



#line 100 "emoji_presentation_scanner.rl"


EMOJI_LINKAGE emoji_text_iter_t
scan_emoji_presentation (emoji_text_iter_t p,
    const emoji_text_iter_t pe,
    bool* is_emoji,
    bool* has_vs)
{
  emoji_text_iter_t ts;
  emoji_text_iter_t te;
  const emoji_text_iter_t eof = pe;

  (void)ts;

  unsigned act;
  int cs;

  
#line 102 "emoji_presentation_scanner.c"
	{
	cs = emoji_presentation_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 108 "emoji_presentation_scanner.c"
	{
	int _slen;
	int _trans;
	const unsigned char *_keys;
	const char *_inds;
	if ( p == pe )
		goto _test_eof;
_resume:
	switch ( _emoji_presentation_from_state_actions[cs] ) {
	case 6:
#line 1 "NONE"
	{ts = p;}
	break;
#line 120 "emoji_presentation_scanner.c"
	}

	_keys = _emoji_presentation_trans_keys + (cs<<1);
	_inds = _emoji_presentation_indicies + _emoji_presentation_index_offsets[cs];

	_slen = _emoji_presentation_key_spans[cs];
	_trans = _inds[ _slen > 0 && _keys[0] <=(*p) &&
		(*p) <= _keys[1] ?
		(*p) - _keys[0] : _slen ];

_eof_trans:
	cs = _emoji_presentation_trans_targs[_trans];

	if ( _emoji_presentation_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _emoji_presentation_trans_actions[_trans] ) {
	case 9:
#line 94 "emoji_presentation_scanner.rl"
	{te = p+1;{ *is_emoji = false; *has_vs = true; return te; }}
	break;
	case 15:
#line 95 "emoji_presentation_scanner.rl"
	{te = p+1;{ *is_emoji = true; *has_vs = true; return te; }}
	break;
	case 4:
#line 96 "emoji_presentation_scanner.rl"
	{te = p+1;{ *is_emoji = true; *has_vs = false; return te; }}
	break;
	case 8:
#line 97 "emoji_presentation_scanner.rl"
	{te = p+1;{ *is_emoji = false; *has_vs = false; return te; }}
	break;
	case 13:
#line 94 "emoji_presentation_scanner.rl"
	{te = p;p--;{ *is_emoji = false; *has_vs = true; return te; }}
	break;
	case 14:
#line 95 "emoji_presentation_scanner.rl"
	{te = p;p--;{ *is_emoji = true; *has_vs = true; return te; }}
	break;
	case 11:
#line 96 "emoji_presentation_scanner.rl"
	{te = p;p--;{ *is_emoji = true; *has_vs = false; return te; }}
	break;
	case 12:
#line 97 "emoji_presentation_scanner.rl"
	{te = p;p--;{ *is_emoji = false; *has_vs = false; return te; }}
	break;
	case 3:
#line 96 "emoji_presentation_scanner.rl"
	{{p = ((te))-1;}{ *is_emoji = true; *has_vs = false; return te; }}
	break;
	case 1:
#line 1 "NONE"
	{	switch( act ) {
	case 2:
	{{p = ((te))-1;} *is_emoji = true; *has_vs = true; return te; }
	break;
	case 3:
	{{p = ((te))-1;} *is_emoji = true; *has_vs = false; return te; }
	break;
	case 4:
	{{p = ((te))-1;} *is_emoji = false; *has_vs = false; return te; }
	break;
	}
	}
	break;
	case 10:
#line 1 "NONE"
	{te = p+1;}
#line 95 "emoji_presentation_scanner.rl"
	{act = 2;}
	break;
	case 2:
#line 1 "NONE"
	{te = p+1;}
#line 96 "emoji_presentation_scanner.rl"
	{act = 3;}
	break;
	case 7:
#line 1 "NONE"
	{te = p+1;}
#line 97 "emoji_presentation_scanner.rl"
	{act = 4;}
	break;
#line 190 "emoji_presentation_scanner.c"
	}

_again:
	switch ( _emoji_presentation_to_state_actions[cs] ) {
	case 5:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 197 "emoji_presentation_scanner.c"
	}

	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _emoji_presentation_eof_trans[cs] > 0 ) {
		_trans = _emoji_presentation_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	}

#line 120 "emoji_presentation_scanner.rl"


  /* Should not be reached. */
  *is_emoji = false;
  *has_vs = false;
  return p;
}
