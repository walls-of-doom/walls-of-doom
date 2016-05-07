from random import randint


if __name__ == '__main__':
    with open('platforms.txt', 'w') as output:
        platform_count = 16
        print('{}'.format(platform_count), file=output)
        for i in range(platform_count):
            print('{} {}'.format(randint(5, 15), randint(1, 3)), file=output)
