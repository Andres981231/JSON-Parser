# JSON Parser


JSON (JavaScript Object Notation) is a lightweight data-interchange format. 
Nowadays it has been widely used as format for configuration files, data exchange
buffers, etc. In this homework, you are required to implement a *fully-compliant*
json parser, which is able to convert json data from plain text file to the accessible
C++ objects. 

For example: given a json file `parse_me.json` containing bellowing content:
```json
{
  "key_1": [1.0, {"inner_key_2": false}, null]
}
```
your parser should able to access all data entries in a handy way, e.g.:
```C++
Json json_obj;
json_obj.parse("parse_me.json");
auto v = json_obj["key_1"][1]["inner_key_2"].get_bool();
// assert_equal(v, false)
```

## Instructions

Json syntax is simple. As illustrated on 
[Json official website](https://json.org), json is majorly built on *two* structures:
* **array**: an ordered collection of *values*, surrounded by square brackets,
  separated by commas, e.g.:
  ```json
  [1, 2, "substring", false]
  ```
* **object**: an unordered collection of key-*value* pairs, surrounded by curly 
  braces, separated by commas, e.g.:
  ```json
  {"key1": 233, "key2": true}
  ```

The **value** in these two structures can be:
* *string* in double quote, with possible backslash escapes, e.g. `"this is a\tjson string"`;
* boolean *true* or *false*, with literal `true` or `false`, respectively;
* *null* type, with literal `null`;
* real *number*, e.g. `12`, `233e-3`, `-0.222e+6`;
* *array*;
* *object*;

You should check [Json official website](https://json.org) for more detailed
and accurate descriptions for these data types.

You may have noticed that, json is based on a recursive syntax. A recursive decent
parser is thus able to parse all specified values and structures. Which means, all 
you need to is firstly design parsing subroutines for all value types, then using
these subroutines to parse whole json content into a tree-structured C++ object,
which should be easily accessed by other C++ programs.

### Design base class for json value type

The ultimate target of our parser is parsing and organizing json values into a
tree-structured object.
We suggest that all nodes in this tree should have uniformed interface.
For example, the user should be able to query the *data type* of any node:
```C++
const Json &n = ...;
auto t = n.get_type();
```

### Design classes for specific data types

There are several points you should take into consideration during designing
these classes:
1. How to design accessors for each of these classes? What signatures should these
   accessors have? For example, `get_double` and `get_int` sound like reasonable
   interface for *number* value type, but should be invalid for *null* type;
2. How to store actual data for each value types? For *null* or *boolean* types,
   this should be trivial. However:
   * For *number* type, should we distinguish between integers and real numbers?
   * For collection type *array* and *object*, things becomes complicated.
     Containers in STL should be instantiated with specific types during *compile
     time*, e.g. `std::vector<float>`. However, json collection types should be able
     to contain nodes with different types. (*Hint: smart pointers to base class.*)
3. Json *string* comes with unicode support. To be short, non-ASCII characters
   are represented as a unsigned *codepoint* in unicode. These codepoints range
   from `0x0` to `0x10FFFF`. 

   Unicode codepoints are represented as escaped sequences in json literal,
   which starts with `\u`, follows by exactly 4 hexadecimal digits.
   You might have noticed that 4 hexadecimal digits are not sufficient to encode
   from `0x0` to `0x10FFFF`. *Surrogate Pairs* are thus used for encoding codepoints
   larger than `0xFFFF`, which separate a codepoint into *high* and *low* surrogate pair
   and encode them respectively. 
   (Check this [Wikipedia page](https://en.wikipedia.org/wiki/UTF-16#U+010000_to_U+10FFFF)
   for details.)

   For example, the codepoint for Emoji beer "🍺" symbol is `0x1F37A`, which is
   larger than `0xFFFF`.
   Thus it is encoded into surrogate pair `0xD83C, 0xDF7A`, and represented as
   `"\uD83C\uDF7A"` in json string literals. You should carefully read though
   above linked Wikipedia page, and identify surrogate pairs from codepoints within
   `0xFFFF`. Also, please note that *not all surrogate pairs are legal*!

   Each of the decoded unicode codepoints is clearly wider than one byte, so you can't directly save
   it in `std::string` or `char[]`. You should instead find a method that *encode* these
   unsigned values into (a sequence of) unsigned bytes. (*Hint: UTF-8, you might
   also want to read though
   [this tutorial](https://github.com/miloyip/json-tutorial/blob/master/tutorial04/tutorial04.md).*)

### Design parsing method for json object class

The root node for a parsed json data tree must be `object` type. We should thus
design a `parse` method for json `object` class, so users can firstly instantiate an 
`object` instance, then call `parse` method to build a json data tree rooted on 
this node.

As mentioned above, a recursive decent parser is enough. Here are several
suggestions in implementing `parse`:
1. You might want to remove all white spaces, e.g. spaces (U+0020), tabs (U+0009),
   carriage returns (CR, U+000D) and line feeds (LF, U+000A) in loaded json content,
   before calling any parsing subroutines;
2. When parsing numbers, you might find STL function 
   [`std::stof`](https://en.cppreference.com/w/cpp/string/basic_string/stof) useful;
3. When parsing strings, take good care of backslash escapes, e.g. `"\n"`, `"\t"`, etc.
   Escaped line breakers and tabs should not be treated as removable whitespaces in step 1.;
