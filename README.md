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
}

# Optional Commas
struct Person {
  name str
  country str
  planet = "Earth" # Default Value
  hair Hair
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
  hair: Hair:LONG,
}

println(character)
```

## Match
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