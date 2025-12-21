# Language Specification

# General notes

## On the matter of case sensitivity
Visual Basic, by default, is not case sensitive. This means keywords as well as variables can be written in any combination of upper and lowercase letters. While this might sound fun, its definitely not recommended. So here are some general style guidelines:
1. Keywords generally have their first letter capitalized, eg. `If`, `Then`, `While`, `Sub`, `Function`
2. Function and Sub names are written in PascalCase
3. Local variable names are written in camelCase and are prefixed with a type specifying letter, eg. `iSomeInt`, `fSomeFloat`, `sSomeString`, `aSomeArray`, `oSomeObject` 

# Functions and Subroutines
In VB, there are different kinds of user defined procedures. The first one of these is called a subroutine. These very simple procedures can take in arguments but generally arent able to return anything. Alternatively, there are also functions, just like subroutines they can take in arguments but can also give back a return value.

By default, VeeBee begins execution at a subroutine called `Main`. Theres no program code allowed in the global namespace, all code needs to be contained inside of a subroutine or function.

## Subroutines
Subroutines can be defined using the `Sub`-keyword. When calling a subroutines, no parenthesis are used when listing its arguments. Alternatively, the `Call`-statement can be used, this does require the use of parenthesis.
```vba
Sub Main()
  SomeOtherSub "some text"
  Call SomeOtherSub("some more text")
End Sub

Sub SomeOtherSub(text)
  MessageBox text
End Sub
```

## Functions
Functions are essentially just subroutines but capable of returning a value. A value is returned by assigning it to the functions name. Whatever value is stored in this special reserved variable when the function call ends will be returned to the functions callsite.

When calling a function whos return value will be used for further computation parenthesis are required. If the return value will be discaded anyways, no parenthesis are required.

```vba
Sub Main()
  MessageBox Add(2, 4)
  Say "look mum no parenthesis"
End Sub

Function Add(a, b)
  Add = a + b
End Function

Function Say(text)
  MessageBox text
  Say = text 
End Function
```
## Types
## Base types

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
| User-Defined | sum of its parts | it really depends tbh | very cool struct |

[^1]: https://stackoverflow.com/questions/18637364/why-is-booleans-size-in-vba-2-bytes

## Structures
VeeBee also allows for user defined structures, the same way as VBA does. Types are defined using the `Type`-keyword, followed by the types fields.

```vba
Type Employee
  Name As String ' Name of the employee
  Age As Integer ' Age of the employee
End Employee
```

# Variables

## Definition of Primitives
```vba
Dim vMyVariable ' define new variable of type 'Variant'
Dim iMyNumer As Integer ' define new variable of type 'Integer' 
Dim oMyObject As Object ' define new variable of type 'Object'
```

## Definition of Arrays
```vba
' Single dimensional arrays
Dim aNumberList(10) As Integer ' create a new array of type Integer of length 10
ReDim aNumberList(15) ' resize our array to be of size 15, this also clears the array
ReDim Preserve aNumberList(15) ' resize our array to be of size 15 and keep the old values

' Multidimensional arrays
Dim a2Pixels(100, 100) As Byte ' declare a matrix of size 100x100
ReDim a2Pixels(200, 200) ' resize our matrix to size 200x200
ReDim Preserve a2Pixels(200, 200) ' resize our matrix and keep our previously set values, if they dont fit they will be truncated

' We can do up to 60 dimensions because hell yeah
Dim a60MyGod(10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10) As Integer

' Index offsets
Dim sDaysOfTheWeek(1 To 7) As String ' will create an array whos first index is 1 and last index is 7
Dim iCalender(1 To 12, 1 To 31) As String ' will create a 2D array with the given ranges
```


# Sources
(some of them are german and i dont care)  
- https://learn.microsoft.com/en-us/office/vba/language/concepts/getting-started/calling-sub-and-function-procedures
- https://learn.microsoft.com/en-us/office/vba/language/reference/user-interface-help/data-type-summary
- https://learn.microsoft.com/en-us/office/vba/language/how-to/user-defined-data-type
- https://learn.microsoft.com/de-de/office/vba/language/concepts/getting-started/using-arrays
