# Language Specification

# Variables

## Datatypes
| Type | Size In Memory | Possible States | Notes |
| ---- | -------------- | --------------- | ----- |
| Boolean | 2 bytes (yes really)[^1] | True / False |  |
| Byte | 1 byte | 0 ... 255 |  |
| Integer | 2 bytes | -32 768 ... 32 767 | |
| Long | 4 bytes | -2 147 483 648 ... 2 147 483 647 | |
| LongLong | 8 bytes | -9 223 372 036 854 775 808 ... 9 223 372 036 854 775 807 | yes this is really what they called it |
| Currency | 8 bytes | -922 337 203 685 477.5808 ... 922 337 203 685 477.5807 | fixed point number with 4 decimal places |
| Decimal | 14 bytes | [Todo] | 29 digits with a moving decimal point |
| Single | 4 bytes | -3.402823E38 to -1.401298E-45 for negative values, 1.401298E-45 to 3.402823E38 for positive values | |
| Double | 8 bytes | -1.79769313486231E308 to -4.94065645841247E-324 for negative values, 4.94065645841247E-324 to 1.79769313486232E308 for positive values | |
| String | 10 bytes + string length | string of 0 to ~2 billion characters |
| Object | 4 bytes | reference to any object | | 
| Collection | dynamic | dynamic | Dynamic size array type |
| Dictionary | dynamic | dynamic | Key/Value storage |
| User-Defined | sum of its parts | idk | very cool struct |

[^1]: https://stackoverflow.com/questions/18637364/why-is-booleans-size-in-vba-2-bytes

## Definition of Primitives
```vb
Dim vMyVariable ' define new variable of type 'Variant'
Dim iMyNumer As Integer ' define new variable of type 'Integer' 
Dim oMyObject As Object ' define new variable of type 'Object'
```

## Definition of Arrays
```vb
' Single dimensional arrays
Dim aNumberList(10) As Integer ' create a new array of type Integer of length 10
ReDim aNumberList(15) ' resize our array to be of size 15, this also clears the array
ReDim Preserve aNumberList(15) ' resize our array to be of size 15 and keep the old values

' Multidimensional arrays
Dim a2Pixels(100, 100) ' declare a Matrix of size 100x100

' Index offsets
Dim sDaysOfTheWeek(1 To 7) ' will create an array whos first index is 1 and last index is 7
Dim iCalender(1 To 12, 1 To 31) ' will create a 2D array with the given ranges
```


# Sources
(some of them are german and i dont care)  
https://learn.microsoft.com/en-us/office/vba/language/reference/user-interface-help/data-type-summary
https://learn.microsoft.com/de-de/office/vba/language/concepts/getting-started/using-arrays
https://learn.microsoft.com/en-us/office/vba/language/reference/user-interface-help/data-type-summary
