import random


if __name__ == '__main__':
    with open('platforms.txt', 'w') as output:
        for i in range(16):
            print('{} {}'.format(random.randint(5, 15), random.randint(1, 3)), file=output)
