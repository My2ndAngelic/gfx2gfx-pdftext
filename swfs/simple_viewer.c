/* simple_viewer.c

   Creates the swf file simple_viewer.swf. 
   I'm not much of an designer. Suggestions and improvements concerning
   this file or simple_viewer.swf are highly welcome. -mk
   
   Part of the swftools package.

   Copyright (c) 2000, 2001 Matthias Kramm <kramm@quiss.org>
 
   This file is distributed under the GPL, see file COPYING for details 
*/

#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include "rfxswf.h"

TAG* tag;

int button_sizex = 20; //button size in pixels
int button_sizey = 20; 

RGBA button_colors[6]=
{{0,0x00,0x00,0x80}, //Idle      left
 {0,0x00,0x00,0x80}, //   "      right
 {0,0x20,0x20,0xc0}, //MouseOver left
 {0,0x20,0x20,0xc0}, //   "      right
 {0,0x10,0x10,0xff}, //Down      left
 {0,0x10,0x10,0xff}};//   "      right

int useDefineButton2 = 0; // set this to 1 to use DefineButton2 Tags
                          // instead of DefineButton1
#define SUBTITLES 1

int main (int argc,char ** argv)
{ SWF swf;
  RGBA rgb;
  SRECT r;
  LPSHAPE s;
  MATRIX m;
  ActionTAG*a1,*a2,*a3;
  S32 width=1024,height = 768;
  
  int f,i,ls1,fs1;
  int count;
  int t;

  memset(&swf,0x00,sizeof(SWF));        // set global movie parameters

  swf.fileVersion    = 4;               // make flash 4 compatible swf
  swf.frameRate      = 0x1900;          // about 0x19 frames per second
  
  swf.movieSize.xmax = 20*width;        // flash units: 1 pixel = 20 units
  swf.movieSize.ymax = 20*height;

  swf.firstTag = swf_InsertTag(NULL,ST_SETBACKGROUNDCOLOR);
  tag = swf.firstTag;
  rgb.r = 0xff;
  rgb.g = 0xff;
  rgb.b = 0xff;
  swf_SetRGB(tag,&rgb);
 
  // initialize matrix
  m.sx = 65536; //scale
  m.sy = 65536;
  m.r0 = 0; //rotate
  m.r1 = 0;
  m.tx = 0; //move
  m.ty = 0;

  /* the "viewport" object will be replaced by swfcombine
     with the object to browse. It is placed at the
     upper left corner (0,0), by setting m.tx and m.ty
     to 0. Therefore, the buttons are "in" the viewport,
     not above it*/
  tag = swf_InsertTag(tag,ST_DEFINESPRITE);
  swf_SetU16(tag, 23); //id
  swf_SetU16(tag, 0); //frames
  tag = swf_InsertTag(tag,ST_END);
  tag = swf_InsertTag(tag,ST_PLACEOBJECT2);
  m.tx = 0; //move
  m.ty = 0;
  swf_ObjectPlace(tag, 23, 1,&m,0,"viewport");

  for(count=0;count<6;count++)
  {
      tag = swf_InsertTag(tag,ST_DEFINESHAPE);
      swf_ShapeNew(&s);               // create new shape instance
      rgb.r = rgb.b = rgb.g = 0x00;           
      ls1 = swf_ShapeAddLineStyle(s,40,&rgb);
      rgb = button_colors[count];
      fs1 = swf_ShapeAddSolidFillStyle(s,&rgb);
      swf_SetU16(tag,32+count);                // now set character ID
      r.xmin = 0;
      r.ymin = 0;
      r.xmax = 20*width;
      r.ymax = 20*height;
      swf_SetRect(tag,&r);              // set shape bounds
      swf_SetShapeHeader(tag,s);        // write all styles to tag
      if(count&1)
      {
	  swf_ShapeSetAll(tag,s,0,0,ls1,fs1,0); // move to (0,0), select linestyle ls1 and no fillstyle
	  /* SetLine coordinates are relative.
	     It's important that the start and end points match, otherwise
	     the Macromedia Flash player will crash. */
	  swf_ShapeSetLine(tag,s,20*button_sizex,20*button_sizey);
	  swf_ShapeSetLine(tag,s,-20*button_sizex,20*button_sizey);
	  swf_ShapeSetLine(tag,s,0,-40*button_sizey);
      } else {
	  swf_ShapeSetAll(tag,s,20*button_sizex,0,ls1,fs1,0);
	  swf_ShapeSetLine(tag,s,-20*button_sizex,20*button_sizey);
	  swf_ShapeSetLine(tag,s,20*button_sizex,20*button_sizey);
	  swf_ShapeSetLine(tag,s,0,-40*button_sizey);
      }
      swf_ShapeSetEnd(tag);   // finish drawing
      swf_ShapeFree(s);   // clean shape structure (which isn't needed anymore after writing the tag)
  }

  a1 = swf_ActionStart(tag);
    action_SetTarget("viewport");
    action_PreviousFrame();
    action_SetTarget("");
    action_End();
  swf_ActionEnd();

  a2 = swf_ActionStart(tag);
    action_SetTarget("viewport");
    action_NextFrame();
    action_SetTarget("");
    action_End();
  swf_ActionEnd();

  a3 = swf_ActionStart(tag);
    action_SetTarget("viewport");
    action_Stop();
    action_SetTarget("");
    action_PushString("/:subtitle");
    action_PushString("...");
    action_SetVariable();
    action_End();
  swf_ActionEnd();

  for(t=0;t<2;t++)
  {
      if(!useDefineButton2)
      {
	  tag = swf_InsertTag(tag,ST_DEFINEBUTTON);
	  swf_SetU16(tag,30+t); //id
	  swf_ButtonSetRecord(tag,BS_UP|BS_HIT,32+t,1,NULL,NULL);
	  swf_ButtonSetRecord(tag,BS_OVER,34+t,1,NULL,NULL);
	  swf_ButtonSetRecord(tag,BS_DOWN,36+t,1,NULL,NULL);
	  swf_SetU8(tag,0); // end of button records
	 
	  if(t)
	    swf_ActionSet(tag,a2);
	  else
	    swf_ActionSet(tag,a1);
      }
      else
      {
	  tag = swf_InsertTag(tag,ST_DEFINEBUTTON2);
	  swf_SetU16(tag,30+t); //id
	  swf_ButtonSetFlags(tag, 0); //menu=no
	  swf_ButtonSetRecord(tag,BS_UP|BS_HIT,32+t,1,NULL,NULL);
	  swf_ButtonSetRecord(tag,BS_OVER,34+t,1,NULL,NULL);
	  swf_ButtonSetRecord(tag,BS_DOWN,36+t,1,NULL,NULL);
	  swf_SetU8(tag,0); // end of button records

	  swf_ButtonSetCondition(tag, BC_OVERDOWN_OVERUP);
	  if(t)
	    swf_ActionSet(tag,a2);
	  else
	    swf_ActionSet(tag,a1);
	   
	  swf_ButtonPostProcess(tag, 1); // don't forget!
      }
  }

  tag = swf_InsertTag(tag,ST_DOACTION);
  swf_ActionSet(tag,a3);

  m.tx = 0; //move
  m.ty = 0;
  tag = swf_InsertTag(tag,ST_PLACEOBJECT2);
  swf_ObjectPlace(tag, 30, 2,&m,0,0);
  m.tx = button_sizex*30;
  tag = swf_InsertTag(tag,ST_PLACEOBJECT2);
  swf_ObjectPlace(tag, 31, 3,&m,0,0);
  
  swf_ActionFree(a1);
  swf_ActionFree(a2);
  swf_ActionFree(a3);

#ifdef SUBTITLES
  tag = swf_InsertTag(tag,ST_DEFINEFONT2); {
      U8 data[] = {0x90, 0x00, 0x0f, 0x54, 0x69, 0x6d, 0x65, 0x73, 
		   0x20, 0x4e, 0x65, 0x77, 0x20, 0x52, 0x6f, 0x6d, 
		   0x61, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		   0x00, 0x00, 0x00, 0x00};
      swf_SetU16(tag, 0x76); //id
      swf_SetBlock(tag, data, sizeof(data));
  }
  tag = swf_InsertTag(tag,ST_DEFINEEDITTEXT); {
      EditTextLayout layout;
      layout.align = 0;
      layout.leftmargin = 0;
      layout.rightmargin = 0;
      layout.indent = 0;
      layout.leading = 0;

      swf_SetU16(tag, 0x77);//id
      r.xmin = button_sizex*60;
      r.xmax = r.xmin+ 826*20;
      r.ymin = 0;
      r.ymax = button_sizey*80;
      rgb.r = rgb.g = rgb.b = 0;
      rgb.a = 255;
      swf_SetEditText(tag, ET_MULTILINE|ET_READONLY, r, 0, &rgb, 0, 0x76, button_sizey*40, &layout, "/:subtitle");
      m.tx = m.ty = 0;
  }
  tag = swf_InsertTag(tag,ST_PLACEOBJECT2);
  swf_ObjectPlace(tag, 0x77, 4,&m,0,0);
#endif

  tag = swf_InsertTag(tag,ST_SHOWFRAME);
  tag = swf_InsertTag(tag,ST_END);
  

  f = open("simple_viewer.swf",O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if FAILED(swf_WriteSWF(f,&swf)) fprintf(stderr,"WriteSWF() failed.\n");
  close(f);

  swf_FreeTags(&swf);                       // cleanup
  return 0;
}


