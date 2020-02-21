player = {
    position = {
    x = 32.5, y = 20.0
    },
    filename = "sprite.png",
    HP = 300
}

array = {1, 1, 2, 3, 5, 10, 20}

function sum(x, y)
    print("printing array: ")
    for a = 1, #array do
        io.write(array[a],",")
    end
    print("")
    return x + y;
end

a = 6