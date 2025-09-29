#pragma once

/**
 * When approximating an arc or circle, should the error be placed on the outside
 * or inside of the curve?  (Generally speaking filled shape errors go on the inside
 * and knockout errors go on the outside.  This preserves minimum clearances.)
 */
enum ERROR_LOC
{
    ERROR_OUTSIDE = 0,
    ERROR_INSIDE
};