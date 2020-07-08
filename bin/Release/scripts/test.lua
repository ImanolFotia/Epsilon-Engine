clock = os.clock

Position = {x = 5.0, y = 2.0, z = 6.0};

function OnInit() 
    return 0;
end

function OnUpdate()
    Position.x = math.sin(Position.x + clock()) +
                 math.cos(Position.x + clock());
    print(Position.x)
    return 0;
end

function OnTerminate()
    return 0;
end

