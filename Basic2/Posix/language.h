/*
 *
 *	$Id: language.h,v 1.2 2023/08/29 06:00:21 stefan Exp stefan $ 
 *
 *	Stefan's IoT BASIC interpreter 
 *
 * 	See the licence file on 
 *	https://github.com/slviajero/tinybasic for copyright/left.
 *    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 *	Author: Stefan Lenz, sl001@serverfabrik.de
 *
 *	This is the language definition file. Edit this to set the language 
 * 	capabilities.
 *
 *	MEMSIZE sets the BASIC main memory to a fixed value,
 *		if MEMSIZE=0 a heuristic is used based on free heap
 *		size and architecture parameters
 */
#define MEMSIZE 0

/*
 *	DEBUG switches on compiled debug mode. Consider using runtime 
 * 		debug with SET 0,x before using this.
 */
#define DEBUG 0

/*
 * Interpreter feature sets, choose one of the predefines 
 * or undefine all predefines and set the features in custom settings
 *
 * BASICFULL: full language set, use this with flash >32kB - ESPs, MKRs, Mega2560, RP2040, UNO R4
 * BASICINTEGER: integer BASIC with full language, use this with flash >32kB
 * BASICSIMPLE: integer BASIC with reduced language set, 32kB capable - for UNOs with a lot of device drivers
 * BASICSIMPLEWITHFLOAT: a small floating point BASIC, 32kB capable, for UNOs - good for UNOs with the need of float
 * BASICTINYWITHFLOAT: a floating point tinybasic, if you have 32kB and need complex device drivers
 * BASICMINIMAL: minimal language, just Palo Alto plus Arduino I/O, works on 168 with 1kB RAM and 16kB flash
 */
#define	BASICFULL
#undef	BASICINTEGER
#undef	BASICSIMPLE
#undef	BASICMINIMAL
#undef	BASICSIMPLEWITHFLOAT
#undef	BASICTINYWITHFLOAT

/*
 * custom settings undef all the the language sets 
 * when you def here
 */ 
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#define HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#define HASARRAYLIMIT
#define HASSTRUCT

/* Palo Alto plus Arduino functions */
#ifdef BASICMINIMAL
#undef HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#undef HASSTEFANSEXT
#undef HASERRORMSG
#undef HASVT52
#undef HASFLOAT
#undef HASGRAPH
#undef HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#undef HASTIMER
#undef HASEVENTS
#undef HASERRORHANDLING
#undef HASARRAYLIMIT
#undef HASSTRUCT
#endif

/* all features minus float and tone */
#ifdef  BASICINTEGER
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#define HASARRAYLIMIT
#define HASSTRUCT
#endif

/* a simple integer basic for small systems (UNO etc) */
#ifdef  BASICSIMPLE
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#undef  HASGRAPH
#define HASDARTMOUTH
#undef  HASDARKARTS
#define HASIOT
#undef  HASMULTIDIM
#undef  HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#undef 	HASARRAYLIMIT
#undef 	HASSTRUCT
#endif

/* all features activated */
#ifdef BASICFULL
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#define HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#define HASARRAYLIMIT
#define HASSTRUCT
#endif

/* a simple BASIC with float support */
#ifdef BASICSIMPLEWITHFLOAT
#define HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#undef HASVT52
#define HASFLOAT
#undef HASGRAPH
#define HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#undef HASTIMER
#undef HASEVENTS
#undef HASERRORHANDLING
#undef HASARRAYLIMIT
#undef HASSTRUCT
#endif

/* a Tinybasic with float support */
#ifdef BASICTINYWITHFLOAT
#define HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#undef HASVT52
#define HASFLOAT
#undef HASGRAPH
#undef HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#undef HASTIMER
#undef HASEVENTS
#undef HASERRORHANDLING
#undef HASARRAYLIMIT
#undef HASSTRUCT
#endif

/* 
 *	Language feature dependencies
 * 
 * Dartmouth and darkarts needs the heap which is in Apple 1
 * IoT needs strings and the heap, also Apple 1
 * 
 * String arrays need multi dimensional capabilities
 *
 * The structured language set needs ELSE from STEFANSEXT
 *
 */
#if defined(HASDARTMOUTH) || defined(HASDARKARTS) || defined(HASIOT)
#define HASAPPLE1
#endif

#if defined(HASSTRINGARRAYS)
#define HASMULTIDIM
#endif

#ifdef HASSTRUCT
#define HASSTEFANSEXT
#endif

/* dependencies on the hardware */

#if !defined(DISPLAYHASGRAPH) 
#undef HASGRAPH
#endif


