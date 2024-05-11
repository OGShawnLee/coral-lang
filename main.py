def print_character(name, game):
    print(f"\t{name} is a cool {game} character")

def main():
    game = input("What game? ")
    match game:
        case "Crysis" | "Halo" | "Gears of War":
            print("Good game!")

        case _:
            print(f"{game} is a decent game!")


    times = int(input("How many characters do you want to add? "))
    if times > 5:
        print("\tToo many characters!")
        print("\tReducing to only 5...")
        times = 5

    collection = [input("What is their name? ") for it in range(times)]
    print(f"Characters from {game}:")
    for character in collection:
        print_character(character, game)


main()
