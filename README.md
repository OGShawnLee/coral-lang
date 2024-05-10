## Variables and Constants

```
var is_married = false
val name = "Shawn"
val last_name = "Lee"
val full_name = "#name #last_name" # String Injection
val country = "China"
var children = 0
var budget = 0.75
```

## Arrays
```
var collection = []int # Empty Array
# Optional Commas
collection = []int {
  len: 12
  init: it * 12 # len must be declared if init is present
}
# mapping
val string = []str {
  len: collection:len
  init: "#it: String"
} 
val matrix = [][]int {
  len: 9
  init: []int { len: 9, init: it + 10 }
}
```

## Conditional
### If and Else Statements
```
val is_single = true
val has_self_love = true
val name = "Shawn"

if is_single {
  println("#name is still single")
}

if is_single {
  println("#name is still single")
} else {
  println("#name got a girlfriend!")
}

if is_single {
  println("#name is still single")
} else if has_self_love {
  println("#name got a girlfriend and has achieved peak happiness!")
} else {
  println("#name got a girlfriend!")
}

val age = 4

if age < 0 {
  println("Huh?")
} else if age < 18 {
  println("Get away you kiddo!")
} else if age < 21 {
  println("Almost there!")
} else {
  println("You can drink")
}
```

### Match
```
val name = "Shawn Lee"

match name {
  # Optional Commas
  when "Alexander" "Shawn" "Shawn Lee" {
    println("Best name there is")
  }
  when 
    "Felipe" 
    "Hugo"
    "Alberto" 
    "Roberto"
  {
    println("What is that goofy name?")
  }
  else {
    println("OK, I guess...")
  }
}
```

## Loop
```
for {
  println("This will run forever!")
}

for 12 {
  println("This will run 12 times")
}

for i in 12 {
  println("This has run for the #i time a total of 12 times")
}

val int_arr = []int { len: 3, init: it + 1 }
for integer in int_arr {
  println(integer)
}
```

## Functions

```
fn print {
  println("This fn has no parameters")
}

fn greet(name str, planet str) {
  return "Hello, #name! You live on planet #planet!"
}

# Optional Commas
fn get_message(
  name str
  planet = "Earth" # Default Value
  is_married = false
) {
  if is_married {
    return "#name lives in planet #planet and is married"    
  } else {
    return "#name lives in planet #planet and is not married"    
  }
}

# Optinal Commas
get_message(
  "Shawn Lee"
  "Mars"
  false
)
```

### Lambdas (Anonymous Functions)
```
fn {
  println("This is a lambda with no parameters")
}

val greet = fn (name str) {
  println("Hello, #name!")
}
```

### Functional Programming
```
fn fold(
  arr []int
  initial int
  fun fn(int, int)
) {
  var acc = initial
  for num in arr {
    acc += fun(num, acc)
  }
  return acc
}

fn map(arr []int, fun fn(int)) {
  return []int {
    len: arr:len
    init: fun(arr:at(it))
  }
}

fn call(fun fn) {
  fun()
}

val arr_int = []int { len: 6, init: it * 3 }
val arr_big = map(arr_int, fn (num int) {
  return num * 10
})
val total = fold(arr_big, 0, fn (num int, acc int) {
  return acc + num
})

call(fn {
  println("Hello!")
})
```

## Enum and Struct
```
# Optional Commas
enum Hair {
  BALD 
  VERY_SHORT 
  SHORT
  MEDIUM
  LONG
  VERY_LONG

  # Enum Method
  fn get_hair(len_cm float) {
    if len_cm < 1  {
      return BALD
    } else if len_cm <= 2.5 {
      return VERY_SHORT
    } else if len_cm <= 5 {
      return SHORT
    } else if len_cm <= 7 {
      return MEDIUM
    } else if len_cm <= 10 {
      return LONG
    } else {
      return VERY_LONG
    }
  }
}

# Optional Commas
struct Person {
  name str
  country str
  planet = "Earth" # Default Value
  hair Hair

  fn greet {
    println("Hello, #name!")
  }

  # Struct Method
  fn move(nation str) {
    country = nation
    println("#name has moved to #nation")
  }
}

# Optional Commas
var character = Person {
  name: "Shawn Lee"
  country: "China"
  hair: Hair:MEDIUM # Enum Accessing (might change in favour of double colon operator)
}

# Property Accessing (might change in favour of dot operator)
println(character:name, character:country)

var character = Person {
  name: "Julia Lee",
  country: "France",
  hair: Hair:get_hair(15.75),
}

character:greet()
println(character)
```