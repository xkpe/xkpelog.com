Best Practices
==============

Variables
---------

### Always use *auto* to create variables

* Except for types that are not moveable

```cpp
auto foo = Foo{ /* parameters */ };
const auto& bar = foo.GetBar();
```

### Give meaningful names to the variables

```cpp
auto cancelButton = Button{};
```

### Variables should be fully initialized after their creation statement

* Don't do 2/3/4/5.. step initialization

### Capture variables by reference

Use const references when data will not be modified
```cpp
const auto& buttonSize = cancelButton.GetSize();
```

Use reference when you want to modify the source value
```cpp
auto& data = propertyBuffer.GetData();
for( auto&& value : data )
{
  value = 0;
}
```

Use *r-value* references for temporary objects that will be moved
```cpp
auto&& okButton = buttonFactory.New();
auto popup = popupFactory.New(std::move(okButton));
```


### Keep variables in the minimum required scope

Classes declaration
-------------------

### Every *class* __must__ declare at least one constructor

* There should be one constructor that initializes members
* If a constructor initializes members it __must__ initialize all of them
* All other constructors should delegate
* Use *default* if the class has no members
* Use *default* if *default* construction of all members is desired

### Initializer lists should use *brace initialization*

### Use *struct* for simple types that do not require a constructor

* Members __must__ be initialized via *uniform initialization*
* __Must__ not declare any constructor

### Declare all copy/move constructors/assignment operators
Class by default should follow the following example:
```cpp
class Foo final {
public:
  Foo() = default;
  Foo( const Foo& ) = delete;
  Foo& operator=( const Foo& ) = delete;
  Foo( Foo&& ) = default;
  Foo& operator=( Foo&& ) = default;
  ~Foo() = default;
};
```

Inheritance
-----------

### Classes should be *final* by default
### If a class is not *final* it __must__ have a virtual destructor
### Use *final* on member functions
### Use *override* on member functions

### Don't user *copydoc* on function overrides
Function overrides are implementing a behaviour that is different from the on in the base class, it should be documented properly.

Code Quality
------------

### Avoid basic types in function parameters

Prefer named types, create aliases, preferably strong types that represent the quantities.

Good:
```cpp
SetRectArea( Corner topLeft, SizeV2 size );
//...
SetRectArea( Corner{ 10, 20 }, SizeV2{ 10, 10 } );

```

Bad:
```cpp
SetRectArea( int left, int top, unsigned width, unsigned height );
SetRectArea( int left, int top, int right, int bottom );
SetRectArea( int left, int right, int top, int bottom );
//...
SetSize( 10, 20, 10, 10 ); // can't really tell what parameters are from this line
```

```cpp
SetRectArea( Vector2 topleft, Vector2 size );
SetRectArea( Vector2 topleft, Vector2 bottomRight );
//..
SetRectArea( Vector2{10, 20}, Vector2{10, 10} ); // still hard to know what it means
```

### Use enums instead of booleans

It's hard to tell what is the meaning of boolean parameters in functions with multiple parameters.

Good:
```cpp
SetVisibilityOptions( DisableState disabled, HiddenState hidden );
//...
SetVisibilityOptions( DisableState::ENABLED, HiddenState::VISIBLE );
```

Bad:
```cpp
SetVisibilityOptions( bool disabled, bool hidden );
SetVisibilityOptions( bool hidden, bool disabled );
SetVisibilityOptions( bool enabled, bool visible );
//...
SetVisibilityOptions( false, false ); // What???
```

### Express logic of constraints only once

Define a type that enforces the condition:
```cpp
template < typename T >
constexpr T NonNegativeNumber( T value )
{
  assert( 0 <= value );
  return value;
}
```

Enforce the constraint in code:
```cpp
template < typename T >
T SquareRoot( T number )
{
  auto nonNegativeNumber = NonNegativeNumber< T >( number );
  return std::sqrt( nonNegativeNumber );
}
```

Transparent for the user:
```cpp
{
  constexpr auto f = 1.234567f;
  std::cout << SquareRoot( f ) << std::endl;
}
{
  constexpr auto f = -1.234567f;
  std::cout << SquareRoot( f ) << std::endl;
}
```

Result:
```bash
$ ./positive 
1.111110687255859375
positive: positive.cpp:10: T NonNegativeNumber(T) [T = float]: Assertion `0 <= value' failed.
Aborted (core dumped)
```

Another example:

```cpp
template < typename T >
constexpr T* NotNull( T* value )
{
  assert( nullptr != value );
  return value;
}
```

### 

Dependency Injection
====================

#


