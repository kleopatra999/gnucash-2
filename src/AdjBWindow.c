/********************************************************************\
 * AdjBWindow.c -- the adjust balance window                        *
 * Copyright (C) 1997 Robin D. Clark                                *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, write to the Free Software      *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        *
 *                                                                  *
 *   Author: Rob Clark                                              *
 * Internet: rclark@cs.hmc.edu                                      *
 *  Address: 609 8th Street                                         *
 *           Huntington Beach, CA 92648-4632                        *
\********************************************************************/

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/LabelGP.h>
#include "main.h"
#include "util.h"
#include "date.h"
#include "Data.h"
#include "MainWindow.h"
#include "Account.h"

/** GLOBALS *********************************************************/
extern Data *data;

/** STRUCTS *********************************************************/
typedef struct _AdjBWindow
{
  Account *acc;             /* The account that we are adjusting    */
  Widget  dialog;           /* The adjust balance dialog            */
  Widget  balance;          /* Text field, the new balance          */
  Widget  date;             /* Text field, the date for the balance */
} AdjBWindow;


/** PROTOTYPES ******************************************************/
void adjBOkCB( Widget mw, XtPointer cd, XtPointer cb );
void adjBClose( Widget mw, XtPointer cd, XtPointer cb );


/********************************************************************\
 * adjBWindow                                                       *
 *   opens up the window to adjust the balance                      *
 *                                                                  *
 * Args:   parent  - the parent of this window                      *
 *         account - the account to adjust                          *
 * Return: recnData - the instance of this AdjBWindow               *
\********************************************************************/
AdjBWindow *
adjBWindow( Widget parent, Account *acc )
  {
  Widget widget, pane, controlform, actionform;
  Date date;
  AdjBWindow *adjBData;
  char buf[BUFSIZE];
  
  setBusyCursor( parent );

  adjBData = (AdjBWindow *)_malloc(sizeof(AdjBWindow));
  adjBData->acc = acc;
  
  /* Create the dialog box... */
  sprintf( buf, "%s: Adjust Balance", acc->accountName );
  
  adjBData->dialog =
    XtVaCreatePopupShell( "dialog", 
			  xmDialogShellWidgetClass,	parent,
			  XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL,
			  XmNtitle,          buf,
			  XmNdeleteResponse, XmDESTROY,
                          XmNtransient, FALSE,  /* allow window to be repositioned */
			  NULL );
  
  /* ... and so memory gets freed: */
  XtAddCallback( adjBData->dialog, XmNdestroyCallback, 
                 adjBClose, (XtPointer)adjBData );
  
  /* Create a PanedWindow Manager for the dialog box... the child 
   * of optiondialog the paned window is the parent of the two 
   * forms which comprise the two areas of the dialog box...
   * The sash is set to minimun size to make it invisible */
  pane = XtVaCreateWidget( "pane", 
                           xmPanedWindowWidgetClass, adjBData->dialog,
                           XmNsashWidth,     1,
                           XmNsashHeight,    1,
                           XmNtraversalOn,   False,
                           NULL );
  
  /** CONTROLFORM ****************************************
   * Create a controlform for control area of dialog box */
  controlform = XtVaCreateWidget( "controlform", 
                                  xmFormWidgetClass, pane,
                                  NULL );
  
  widget = XtVaCreateManagedWidget( "Date",
                                    xmLabelGadgetClass, controlform,
                                    XmNtopAttachment,   XmATTACH_FORM,
                                    XmNtopOffset,       10,
                                    XmNrightAttachment, XmATTACH_POSITION,
                                    XmNrightPosition,   50,
                                    NULL );
  
  todaysDate(&date);
  sprintf(buf,"%2d/%2d/%4d\0", date.month, date.day, date.year);
  
  adjBData->date =
    XtVaCreateManagedWidget( "text",
			     xmTextWidgetClass,  controlform,
			     XmNvalue,           buf,
			     XmNtopAttachment,   XmATTACH_FORM,
			     XmNtopOffset,       10,
			     XmNleftAttachment,  XmATTACH_POSITION,
			     XmNleftPosition,    50,
			     NULL );
  
  /* The dateCB ensures the data entered in the date field is
   * in a valid format */
  XtAddCallback( adjBData->date, XmNmodifyVerifyCallback,
                 dateCB, (XtPointer)NULL );
  
  widget = XtVaCreateManagedWidget( "New Balance $",
                                    xmLabelGadgetClass, controlform,
                                    XmNtopAttachment,   XmATTACH_WIDGET,
                                    XmNtopWidget,       adjBData->date,
                                    XmNrightAttachment, XmATTACH_POSITION,
                                    XmNrightPosition,   50,
                                    NULL );
  
  adjBData->balance =
    XtVaCreateManagedWidget( "text",
			     xmTextWidgetClass,  controlform,
			     XmNtopAttachment,   XmATTACH_WIDGET,
			     XmNtopWidget,       adjBData->date,
			     XmNleftAttachment,  XmATTACH_POSITION,
			     XmNleftPosition,    50,
			     NULL );
  
  /* The amountCB ensures the data entered in the balance field is
   * in a valid format */
  XtAddCallback( adjBData->balance, XmNmodifyVerifyCallback,
                 amountCB, (XtPointer)NULL );
  
  XtManageChild( controlform );
  
  /** ACTIONFORM ********************************************
   * Create a Form actionform for action area of dialog box */
  actionform = XtVaCreateWidget( "actionform", 
                                 xmFormWidgetClass, pane,
                                 XmNfractionBase,   8,
                                 NULL );
  
  /* The OK button is anchored to the form, between divider 1 & 2
   * (in the fraction base) */
  widget = XtVaCreateManagedWidget( "Ok", 
                                    xmPushButtonWidgetClass, actionform,
                                    XmNtopAttachment,      XmATTACH_FORM,
                                    XmNbottomAttachment,   XmATTACH_FORM,
                                    XmNleftAttachment,     XmATTACH_POSITION,
                                    XmNleftPosition,       1,
                                    XmNrightAttachment,    XmATTACH_POSITION,
                                    XmNrightPosition,      3,
                                    XmNshowAsDefault,      True,
                                    NULL );
  
  XtAddCallback( widget, XmNactivateCallback,
                 adjBOkCB, (XtPointer)adjBData );
  XtAddCallback( widget, XmNactivateCallback,
                 destroyShellCB, (XtPointer)(adjBData->dialog) );
  
  /* The cancel button! */
  widget = XtVaCreateManagedWidget( "Cancel", 
                                    xmPushButtonWidgetClass, actionform,
                                    XmNtopAttachment,      XmATTACH_FORM,
                                    XmNbottomAttachment,   XmATTACH_FORM,
                                    XmNleftAttachment,     XmATTACH_POSITION,
                                    XmNleftPosition,       3,
                                    XmNrightAttachment,    XmATTACH_POSITION,
                                    XmNrightPosition,      5,
                                    XmNshowAsDefault,      True,
                                    NULL );
  
  XtAddCallback( widget, XmNactivateCallback, 
                 destroyShellCB, (XtPointer)(adjBData->dialog) );  
  
  /* A help button will pop-up context sensitive help */
  widget = XtVaCreateManagedWidget( "Help", 
                                    xmPushButtonWidgetClass, actionform,
                                    XmNtopAttachment,      XmATTACH_FORM,
                                    XmNbottomAttachment,   XmATTACH_FORM,
                                    XmNleftAttachment,     XmATTACH_POSITION,
                                    XmNleftPosition,       5,
                                    XmNrightAttachment,    XmATTACH_POSITION,
                                    XmNrightPosition,      7,
                                    XmNshowAsDefault,      True,
                                    NULL );
  
  XtAddCallback( widget, XmNactivateCallback,
                 helpMenubarCB, (XtPointer)HMB_ADJBWIN );
  
  /* Fix action area of the pane to its current size, and not let it
   *  resize. */
  XtManageChild( actionform );
  {
  Dimension h;
  XtVaGetValues( widget, XmNheight, &h, NULL );
  XtVaSetValues( actionform, XmNpaneMaximum, h, XmNpaneMinimum, h, NULL );
  }
  
  XtManageChild( pane );
  XtPopup( adjBData->dialog, XtGrabNone );
  
  unsetBusyCursor( parent );
  
  return adjBData;
  }

/********************************************************************\
 * adjBClose                                                        *
 *   frees memory allocated for an adjBWindow, and other cleanup    *
 *   stuff                                                          *
 *                                                                  *
 * Args:   mw - the widget that called us                           *
 *         cd - adjBData - the data struct for this window          *
 *         cb -                                                     *
 * Return: none                                                     *
\********************************************************************/
void 
adjBClose( Widget mw, XtPointer cd, XtPointer cb )
  {
  AdjBWindow *adjBData = (AdjBWindow *)cd;
  Account *acc = adjBData->acc;
  
  _free(adjBData);
  acc->adjBData = NULL;
  
  DEBUG("closed AdjBWindow");
  }

/********************************************************************\
 * adjBOkCB                                                         *
 *   creates the new transaction to adjust the account when the     *
 *   user clicks "Ok"                                               *
 *                                                                  *
 * Args:   mw - the widget that called us                           *
 *         cd - adjBData - the data struct for this window          *
 *         cb -                                                     *
 * Return: none                                                     *
\********************************************************************/
void 
adjBOkCB( Widget mw, XtPointer cd, XtPointer cb )
  {
  AdjBWindow  *adjBData = (AdjBWindow *)cd;
  Transaction *trans, *tempTrans;
  Account *acc;
  String   str;
  int      dollar=0,cent=0;
  int      pos=0;
  int      amount=0,currAmount=0;
  int      i;
  
  data->saved = False;
  acc = adjBData->acc;
  
  /* allocate mem for the new transaction */
  trans   = (Transaction *)_malloc(sizeof(Transaction));
  
  /* Create the "trans" transaction */
  str = XmTextGetString(adjBData->date);
  todaysDate(&(trans->date)); /* In case the date field is empty */
  sscanf( str, "%d/%d/%d", &(trans->date.month), 
          &(trans->date.day), &(trans->date.year) );
  str = XmTextGetString(adjBData->balance);
  sscanf( str, "%d.%2d", &dollar, &cent );
  amount = 100*dollar + cent;
  
  /* fill out the rest of the fields */
  trans->num         = XtNewString("");
  trans->memo        = XtNewString("");
  trans->description = XtNewString("Adjust Balance");
  trans->catagory    = 0;
  trans->reconciled  = NREC;
  
  pos = insertTransaction( acc, trans );
  
  /* figure out what the amount for this transaction... figure out
   * the current balance, and take the diff from amount */
  for( i=0; i<pos; i++ )
    {
    tempTrans = getTransaction(acc,i);
    currAmount += tempTrans->amount;
    }
  trans->amount = amount - currAmount;
  
  /* Refresh the account register window */
  regRefresh(acc->regData);
  /* Refresh the account reconcile window */
  recnRefresh(acc->recnData);
  
  refreshMainWindow();
  }
