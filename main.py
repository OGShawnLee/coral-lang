def print_character(name, game):
    print(f"\t{name} is a cool {game} character")

game = input("What game? ")
times = int(input("How many characters do you want to add? "))
if times > 5:
    print("\tToo many characters!")
    print("\tReducing to only 5...")
    times = 5

collection = []
for _ in range(times):
    collection.append(input("What is their name? "))

print(f"Characters from {game}:")
for character in collection:
    print_character(character, game)

