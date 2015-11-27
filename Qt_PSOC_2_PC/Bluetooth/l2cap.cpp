#include "l2cap.h"
//#include <BthDdi.h>


/*************************************************************************
*
* Function:		L2CAP_registerServer()
*
* Description:	Send a 'Register Server' request to receive incoming
*               L2CAP connection requests fromanyremote device for a
*               particular PSM.
*
* Notes:		https://msdn.microsoft.com/en-us/library/windows/hardware/ff536572(v=vs.85).aspx
*
* Parameters:	None.
*
* Returns:		None.
*
**************************************************************************/
void L2CAP_registerServer()
{
    ///////////////////////////////////////////////////////////////////////
    // Register server request.
    ///////////////////////////////////////////////////////////////////////
    // Allocate an IRP.


    // Allocate a BRB.


    // Initialize the parameters of the BRB.


    // Initialize the parameters of the IRP.


    // Pass the IRP down the driver stack.



    ///////////////////////////////////////////////////////////////////////
    // Register the L2CAP callback function.
    ///////////////////////////////////////////////////////////////////////

}
