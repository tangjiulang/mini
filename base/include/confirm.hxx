#ifndef __INCLUDE__CONFIRM_H__
#define __INCLUDE__CONFIRM_H__

#include <string>
#include <QWidget>



/*
 * Display a dialog indicating the file is already open, with an option to reset the lock.
 * @return true if the lock was reset.
 */
bool AskOverrideLock(QWidget* aParent, const QString& aMessage );


/*
 * Display a dialog with Save, Cancel and Discard Changes buttons.
 *
 * @param aParent = the parent window
 * @param aMessage = the main message to put in dialog
 * @param aSaveFunction = a function to save changes, if requested.  Must return true if
 *                        the save was successful and false otherwise (which will result
 *                        in HandleUnsavedChanges() returning wxID_CANCEL).
 * @return wxID_YES, wxID_CANCEL, wxID_NO.
 */
bool HandleUnsavedChanges(QWidget* aParent, const QString& aMessage,
                                        const std::function<bool()>& aSaveFunction );


/*
 * A specialized version of HandleUnsavedChanges which handles an apply-to-all checkbox.
 *
 * @param aParent = the parent window
 * @param aMessage = the main message to put in dialog
 * @param aApplyToAll = if non-null an "Apply to all" checkbox will be shown and it's value
 *                      written back to the bool.
 * @return wxID_YES, wxID_CANCEL, wxID_NO.
 */
int UnsavedChangesDialog(QWidget* aParent, const QString& aMessage,
                                       bool* aApplyToAll );

int UnsavedChangesDialog(QWidget* aParent, const QString& aMessage );


/*
 * Display a confirmation dialog for a revert action.
 */
bool ConfirmRevertDialog(QWidget* parent, const QString& aMessage );


/*
 * Display an error or warning message box with \a aMessage.
 */
void DisplayError(QWidget* aParent, const QString& aText );

/*
 * Display an error message with \a aMessage
 *
 * @param aParent is the parent window
 * @param aMessage is the message text to display
 * @param aExtraInfo is extra data that can be optionally displayed in a collapsible pane
 */
void DisplayErrorMessage(QWidget* aParent, const QString& aMessage,
                                       const QString& aExtraInfo = "" );


/*
 * Display an informational message box with \a aMessage.
 *
 * @param aParent is the parent window
 * @param aMessage is the message text to display
 * @param aExtraInfo is the extra data that can be optionally displayed in a collapsible pane
 */
void DisplayInfoMessage(QWidget* parent, const QString& aMessage,
                                      const QString& aExtraInfo = "");

/*
 * Display a yes/no dialog with \a aMessage and returns the user response.
 *
 * @param aParent is the parent window.  NULL can be used if the parent is the top level window.
 * @param aMessage is the message to display in the dialog box.
 *
 * @return True if user selected the yes button, otherwise false.
 */
bool IsOK(QWidget* aParent, const QString& aMessage );

/*
 * Display a warning dialog with \a aMessage and returns the user response.
 *
 * @param aParent is the parent window.  NULL can be used if the parent is the top level window.
 * @param aWarning is the warning to display in the top part of the dialog box using a bold font.
 * @param aMessage is the message to display in the lower part of the dialog box using the
 *                 default system UI font.
 * @param aDetailedMessage is the message to display in the "Show detailed information" section.
 *                         Passing wxEmptyString will hide this portion of the dialog.
 * @param aOKLabel is the text to display in the OK button.
 * @param aCancelLabel is the text to display in the cancel button.
 *
 * @return wxID_OK or wxID_CANCEL depending on the button the user selected.
 */
int OKOrCancelDialog(QWidget* aParent, const QString& aWarning,
                                   const QString& aMessage,
                                   const QString& aDetailedMessage = "",
                                   const QString& aOKLabel = "",
                                   const QString& aCancelLabel = "",
                                   bool* aApplyToAll = nullptr );



/*
 * Display a dialog with radioboxes asking the user to select an option.
 *
 * @param aParent is the parent window.
 * @param aTitle is the dialog title.
 * @param aMessage is a text label displayed in the first row of the dialog.
 * @param aOptions is a vector of possible options.
 * @return Index of the selected option or -1 when the dialog has been canceled.
 */
int SelectSingleOption(QWidget* aParent, const QString& aTitle,
                                     const QString& aMessage,
                                     const QStringList& aOptions );


#endif /* __INCLUDE__CONFIRM_H__ */
