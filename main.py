def print_character(name, game):
    print(f"\t{name} is a cool {game} character")

def main():
    game = input("What game? ")
    times = int(input("How many characters do you want to add? "))
    if times < 0:
        print("You don't wanna do anything, do you?")
    else:
        if times > 5:
            print("\tToo many characters!")
            print("\tReducing to only 5...")
            times = 5


    collection = [input("What is their name? ") for it in range(times)]
    print(f"Characters from {game}:")
    for character in collection:
        print_character(character, game)


main()
