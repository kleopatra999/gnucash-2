/********************************************************************\
 * Account.h -- the Account data structure                          *
 * Copyright (C) 1997 Robin D. Clark                                *
 * Copyright (C) 1997-2000 Linas Vepstas <linas@linas.org>          *
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
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

#ifndef __XACC_ACCOUNT_H__
#define __XACC_ACCOUNT_H__

#include "config.h"

#include "GNCId.h"
#include "Transaction.h"
#include "kvp_frame.h"


/** PROTOTYPES ******************************************************/

/*
 * The account types are used to determine how the transaction data
 * in the account is displayed.   These values can be safely changed
 * from one release to the next.  Note that if values are added,
 * the file IO translation routines need to be updated. Note 
 * also that GUI code depends on these numbers.
 *
 * ***IMPORTANT***: If you do change the enumeration names (not the
 * numbers), you need to update xaccAccountTypeEnumAsString --- used
 * for text file exports */

typedef enum 
{
  BAD_TYPE = -1,
  NO_TYPE = -1,
  /* Not a type */

  BANK = 0,
  /* The bank account type denotes a savings or checking account
   * held at a bank.  Often interest bearing.
   */

  CASH = 1,
  /* The cash account type is used to denote a shoe-box or pillowcase
   * stuffed with cash.
   */

  CREDIT = 3,  
  /* The Credit card account is used to denote credit (e.g. amex) and 
   * debit (e.g. visa, mastercard) card accounts 
   */

  ASSET = 2,
  LIABILITY = 4,
  /* asset and liability accounts indicate generic, generalized accounts
   * that are none of the above.
   */

  STOCK = 5,
  MUTUAL= 6, 
  /* Stock and Mutual Fund accounts will typically be shown in registers
   * which show three columns: price, number of shares, and value.
   */

  CURRENCY = 7, 
  /* The currency account type indicates that the account is a
   * currency trading account.  In many ways, a currency trading
   * account is like a stock trading account, where both quantities
   * and prices are set.
   */

  INCOME = 8,
  EXPENSE = 9,
  /* Income and expense accounts are used to denote income and expenses.
   * Thus, when data in these accountsare displayed, the sign of the
   * splits (entries) must be reversed.
   */ 

  EQUITY = 10,
  /* Equity account is used to balance the balance sheet. */

  NUM_ACCOUNT_TYPES = 11,
  /* stop here; the following types just aren't ready for prime time */

  /* bank account types */
  CHECKING = 11,
  SAVINGS = 12,
  MONEYMRKT = 13,
  CREDITLINE = 14,     /* line of credit */
} GNCAccountType;

char * xaccAccountGetTypeStr (int type); /* GUI names */

/* Conversion routines for the account types to/from strings.
   Critical for the text communication mechanisms. i.e. INCOME ->
   "INCOME". */
char *   xaccAccountTypeEnumAsString (int type); 
gboolean xaccAccountStringToType (const char* str, int *type);

gboolean xaccAccountTypesCompatible (int parent_type, int child_type);

Account     *xaccMallocAccount (void);
void         xaccFreeAccount (Account *account);

/* Compare two accounts for equality - this is a deep compare. */
gboolean xaccAccountEqual(Account *a, Account* b, gboolean check_guids);

/* 
 * The xaccAccountBeginEdit() and xaccAccountCommitEdit() subroutines
 * provide a pseudo-two-phase-commit wrapper for account updates. 
 * They are mildly useful for detecting attempted updates outside
 * of their scope. However, they do not provide any true two-phase-anything
 * in the current implementation.
 */
void         xaccAccountBeginEdit (Account *account);
void         xaccAccountCommitEdit (Account *account);

kvp_frame * xaccAccountGetSlots (Account *account);

/*
 * The xaccAccountGetGUID() subroutine will return the
 *    globally unique id associated with that account.
 *    User code should use this id to reference accounts
 *    and *not* the integer account id below.
 *
 * The xaccAccountLookup() subroutine will return the
 *    account associated with the given id, or NULL
 *    if there is no such account.
 */
const GUID * xaccAccountGetGUID (Account *account);
Account    * xaccAccountLookup (const GUID *guid);

/*
 * The xaccAccountInsertSplit() method will insert the indicated
 *    split into the indicated account.  If the split already 
 *    belongs to another account, it will be removed from that
 *    account first.
 */
void         xaccAccountInsertSplit (Account *account, Split *split);

/* The xaccAccountFixSplitDateOrder() subroutine checks to see if 
 *    a split is in proper sorted date order with respect 
 *    to the other splits in this account.
 *
 * The xaccTransFixSplitDateOrder() checks to see if 
 *    all of the splits in this transaction are in
 *    proper date order.
 */
void         xaccAccountFixSplitDateOrder (Account *account, Split *split);
void         xaccTransFixSplitDateOrder (Transaction *trans);

/* The xaccIsAccountInList() subroutine returns the number of times
 *    that an account appears in the account list. 
 */
int          xaccIsAccountInList (Account * account, Account **list);

/* The xaccAccountOrder() subroutine defines a sorting order 
 *    on accounts.  It takes pointers to two accounts, and
 *    returns -1 if the first account is "less than" the second,
 *    returns +1 if the first is "greater than" the second, and
 *    0 if they are equal.  To determine the sort order, first
 *    the account codes are compared, and if these are equal, then 
 *    account types, and, if these are equal, the account names.
 */
int          xaccAccountOrder (Account **account_1, Account **account_2);

/* The xaccAccountAutoCode() method will assign an automatically
 *    generated account code to the account, if one does not already 
 *    exist.  Account codes will have the indicated number of digits
 *    in them.  The numbering scheme roughly follows generally
 *    accepted accounting practice, in that top-level accounts
 *    will be number 100, 200, etc., second level accounts 110, 120,
 *    .. 210, 220, ...etc. and third level accounts 111, 112, .. etc.
 */
void         xaccAccountAutoCode (Account *account, int digits);

void xaccAccountSetType (Account *account, int);
void xaccAccountSetName (Account *account, const char *name);
void xaccAccountSetCode (Account *account, const char *code);
void xaccAccountSetDescription (Account *account, const char *desc);
void xaccAccountSetNotes (Account *account, const char *notes);
void xaccAccountSetCurrency (Account *account, const gnc_commodity *currency);
void xaccAccountSetSecurity (Account *account, const gnc_commodity *security);
void xaccAccountSetCurrencySCU (Account *account, int frac);
void xaccAccountSetSecuritySCU (Account *account, int frac);
int  xaccAccountGetCurrencySCU (Account *account);
int  xaccAccountGetSecuritySCU (Account *account);

GNCAccountType xaccAccountGetType (Account *account);
const char *   xaccAccountGetName (Account *account);
const char *   xaccAccountGetCode (Account *account);
const char *   xaccAccountGetDescription (Account *account);
const char *   xaccAccountGetNotes (Account *account);
const gnc_commodity * xaccAccountGetCurrency (Account *account);
const gnc_commodity * xaccAccountGetSecurity (Account *account);
const gnc_commodity * xaccAccountGetEffectiveSecurity (Account *account);

AccountGroup * xaccAccountGetChildren (Account *account);
AccountGroup * xaccAccountGetParent (Account *account);
Account *      xaccAccountGetParentAccount (Account *account);

/* deprecated old double API : this will go away! */
double          DxaccAccountGetBalance (Account *account);
double          DxaccAccountGetClearedBalance (Account *account);
double          DxaccAccountGetReconciledBalance (Account *account);
double          DxaccAccountGetShareBalance (Account *account);
double          DxaccAccountGetShareClearedBalance (Account *account);
double          DxaccAccountGetShareReconciledBalance (Account *account);

gnc_numeric     xaccAccountGetBalance (Account *account);
gnc_numeric     xaccAccountGetClearedBalance (Account *account);
gnc_numeric     xaccAccountGetReconciledBalance (Account *account);
gnc_numeric     xaccAccountGetShareBalance (Account *account);
gnc_numeric     xaccAccountGetShareClearedBalance (Account *account);
gnc_numeric     xaccAccountGetShareReconciledBalance (Account *account);

Split *         xaccAccountGetSplit (Account *account, int i);
int             xaccAccountGetNumSplits (Account *account);

GList*          xaccAccountGetSplitList (Account *account);

gboolean        xaccAccountGetTaxRelated (Account *account);
void            xaccAccountSetTaxRelated (Account *account,
                                          gboolean tax_related);

/* The xaccAccountGetFullName routine returns the fully qualified name
 * of the account using the given separator char. The name must be freed
 * after use. The fully qualified name of an account is the concatenation
 * of the names of the account and all its ancestor accounts starting with
 * the topmost account and ending with the given account. Each name is
 * separated by the given character.
 *
 * WAKE UP!
 * Unlike all other gets, the string returned by xaccAccountGetFullName() 
 * must be freed by you the user !!!
 * hack alert -- since it breaks the rule of string allocation, maybe this
 * routine should not be in this library, but some utility library?
 */
char *         xaccAccountGetFullName (Account *account, const char separator);

/* The IthAccount() routine merely dereferences: the returned
 *    value is just list[i].  This routine is needed for the perl 
 *    swig wrappers, which cannot dereference a list.
 */
Account *      IthAccount (Account **list, int i);

/* xaccAccountsHaveCommonCurrency returns true if the two given accounts
 * have a currency in common, i.e., if they can have common transactions.
 * Useful for UI sanity checks.
 */
gboolean       xaccAccountsHaveCommonCurrency(Account *account_1,
                                              Account *account_2);

/* Returns true if the account has 'ancestor' as an ancestor.
 * Returns false if either is NULL. */
gboolean       xaccAccountHasAncestor (Account *account, Account *ancestor);

/* Get and Set a mark on the account.  The meaning of this mark is
 * completely undefined. Its presented here as a utility for the
 * programmer, to use as desired.  Handy for performing customer traversals
 * over the account tree.  The mark is *not* stored in the database/file
 * format.  When accounts are newly created, the mark is set to zero.
 *
 * The xaccClearMark will find the topmost group, and clear the mark in
 * the entire group tree.  
 * The xaccClearMarkDown will clear the mark only in this and in
 * sub-accounts.
 */
short          xaccAccountGetMark (Account *account); 
void           xaccAccountSetMark (Account *account, short mark); 
void           xaccClearMark (Account *account, short val);
void           xaccClearMarkDown (Account *account, short val);
void           xaccClearMarkDownGr (AccountGroup *group, short val);

/* The xaccAccountSetPriceSrc() and xaccAccountGetPriceSrc() routines
  are used to get and set a string that identifies the current source
  for investment pricing info.  Currently supported values include
  "yahoo", "fidelity", "troweprice", etc.
 
  Since prices are not going to be stored in the accounts in the
  future, and since the whole commodities infrastructure is changing
  radically as we speak, this interface is not long for this world. */

void         xaccAccountSetPriceSrc (Account *account, const char *src);
const char * xaccAccountGetPriceSrc (Account *account);

void xaccAccountSortSplits(Account *account);

gpointer xaccAccountForEachSplit(Account *account,
                                 gpointer (*thunk)(Split *s, gpointer data),
                                 gpointer data);

/* Traverse all of the transactions in the given account.  Continue
   processing IFF proc does not return FALSE. This function does not
   descend recursively to traverse transactions in child accounts.

   Proc will be called exactly once for each transaction that is
   pointed to by at least one split in the given account.

   Note too, that if you call this function on two separate accounts
   and those accounts share transactions, proc will be called once per
   account for the shared transactions.
   
   The result of this function will not be FALSE IFF every relevant
   transaction was traversed exactly once.  */
gboolean
xaccAccountForEachTransaction(Account *account,
                              gboolean (*proc)(Transaction *t, void *data),
                              void *data);

/* Visit every transaction in the account that hasn't already been
   visited exactly once.  visited_txns must be a hash table created
   via guid_hash_table_new() and is the authority about which
   transactions have already been visited.  Further, when this
   procedure returns visited_txns will have been modified to reflect
   all the newly visited transactions.

   The result of this function will not be FALSE IFF every relevant
   transaction was traversed exactly once.  */
gboolean
xaccAccountVisitUnvisitedTransactions(Account *account,
                                      gboolean (*proc)(Transaction *t,
                                                       void *data),
                                      void *data,
                                      GHashTable *visited_txns);

#endif /* __XACC_ACCOUNT_H__ */
