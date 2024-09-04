print("Hi there, want to know if you were born during a leap year? try me")

year = input("Please enter year of birth: ")

if year.isdigit() and len(str(year)) != 4:
    print("not valid")
    print(len(year))
else:
    if (int(year) % 4) == 0:
        print(f"{year} is a leap year")
    elif (int(year) % 100) == 0 and (int(year)) % 400 != 0:
        print(f"{year} is not a leap year")
    elif (int(year) % 400) == 0:
        print(f"{year} is a leap year")
    else:
        print("not a leap year")