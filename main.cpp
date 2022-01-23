#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <array>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


// std::array<double, 2> calculate(const std::array<double, 2>& pos, const std::array<double, 3>& primary_charge)
// {

//     double dx = pos[0] - primary_charge[0] - 20;
//     double dy = pos[1] - primary_charge[1] - 20;

//     double E = primary_charge[2] / (dx * dx + dy * dy);
//     double alpha = std::atan2(dy, dx);
//     double Ex = E * std::cos(alpha);
//     double Ey = E * std::sin(alpha);

//     return {Ex, Ey};

// }

std::array<double, 2> calculate(const std::array<double, 2>& pos, const std::array<double, 3>& primary_charge)
{//Zetko's vector function

        double dx = pos[0] - primary_charge[0] - 20;
        double dy = pos[1] - primary_charge[1] - 20;
        double abs = std::sqrt(dy * dy + dx * dx);
        dy = primary_charge[2] * (dy)/(abs*abs*abs);
        dx = primary_charge[2] * (dx)/(abs*abs*abs);


    return {dx, dy};
}

int select(const std::array<double, 2> pos, const std::vector<std::array<double, 3>>& charges, double r)
{
    for(int i=0; i < charges.size(); i++)
    {   
        const std::array<double, 3>& c = charges[i];
        double d = (pos[0]- c[0] - 20) * (pos[0] - c[0] - 20) + (pos[1] - c[1] - 20) * (pos[1] - c[1] - 20);

        if(d < (r * r))
        {
            return i;
        }
    }
    return std::numeric_limits<int>::max();;
}


int main()
{
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text text;

    double dy = 1; double dx = 3; double sx = 0;
    double x = 1000;double y = 600;
    sf::RenderWindow window(sf::VideoMode(x, y), "Electric Field", sf::Style::Close);

    //-1 -> pos, 1 -> neg
    std::vector<std::array<double, 3>> charges = {{x / 2 - 220, y / 2 - 10, -1}, {x / 2 + 180, y / 2 - 10, +1}};
    double r = 20;
    double precision = 16;
    int iterations = 5000;

    bool frame = true;
    bool clicked = false;
    int selected;
    sf::VertexArray points;
    
    while(window.isOpen())
    {   
        window.clear(sf::Color::Black);

        if(clicked)
        {
            sf::Vector2i position = sf::Mouse::getPosition(window);
            if(charges[selected][0] != position.x && charges[selected][1] != position.y)
            {
                charges[selected][0] = position.x;
                charges[selected][1] = position.y;
                frame = true;
            }
        }

        if(frame)
        {
            points.clear();

            for(double j=0; j <= y; j+=y/precision)
            {
                for (double i=0; i <= x; i+=x/precision)
                {   
                    std::array<double, 2> point = {i, j};

                    for(int k=0; k < iterations; k++)
                    {
                        double ex = 0;double ey = 0;
                        
                        for(std::array<double, 3> c : charges)
                        {
                            std::array<double, 2> xy = calculate(point, c);
                            ex += xy[0];ey += xy[1];
                        }

                        point[0] += ex * 1000;
                        point[1] += ey * 1000;

                        if(point[0] < x && point[1] < y) points.append(sf::Vertex({(float)point[0], (float)point[1]}));
                    }
                }
            }
            frame = false;
        }
        window.draw(points);


        for(std::array<double, 3> c: charges)
        {
            sf::CircleShape shape(20);
            shape.setPosition(c[0], c[1]);
            sf::Color color((0 < c[2]) ? 255 : 0, (c[2] == 0) ? 255 : 0, (c[2] < 0) ? 255 : 0);
            shape.setFillColor(color);


            text.setString((0 < c[2]) ? "+" : (c[2] == 0) ? "o" : "-");

            double dx = (0 < c[2]) ? 10 : (c[2] == 0) ? 11 : 14;
            double dy = (0 < c[2]) ? -3 : (c[2] == 0) ? -3 : -4;
            text.setPosition(c[0] + dx, c[1] + dy);

            text.setCharacterSize(33);text.setFont(font);text.setFillColor(sf::Color::White);text.setStyle(sf::Text::Bold);


            window.draw(shape);
            window.draw(text);
        }


        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                {

                    sf::Vector2i position = sf::Mouse::getPosition(window);
                    selected = select({(double)position.x, (double)position.y}, charges, r);
                    if(selected < charges.size())
                    {
                        clicked = true;
                        frame = true;
                        precision -= 2;
                    }
                    break;
                }
                case sf::Event::MouseButtonReleased:
                {
                    if(clicked)
                    {
                        clicked = false;
                        precision += 2;
                        frame = true;
                    }
                    
                    break;
                }

                case sf::Event::KeyPressed:
                {
                    if(event.key.code == sf::Keyboard::A)
                    {
                        sf::Vector2i position = sf::Mouse::getPosition(window);
                        std::array<double, 3> a = {(double)position.x, (double)position.y, 1};
                        charges.push_back(a);
                        frame = true;
                    }else if(event.key.code == sf::Keyboard::E)
                    {
                        sf::Vector2i position = sf::Mouse::getPosition(window);
                        selected = select({(double)position.x, (double)position.y}, charges, r);
                        if(selected < charges.size())charges.erase(charges.begin() + selected);
                        
                        frame = true;
                    }else if(event.key.code == 55)
                    {
                        sf::Vector2i position = sf::Mouse::getPosition(window);
                        selected = select({(double)position.x, (double)position.y}, charges, r);
                        if(selected < charges.size())charges[selected][2] = round(charges[selected][2] * 10 + 1) / 10;
                        
                        frame = true;
                    }else if(event.key.code == 56)
                    {
                        sf::Vector2i position = sf::Mouse::getPosition(window);
                        selected = select({(double)position.x, (double)position.y}, charges, r);
                        if(selected < charges.size())charges[selected][2] = round((charges[selected][2] * 10 - 1)) / 10;
                        
                        frame = true;
                    }
                    break;
                }

                default:break;
            }
           
        }

        window.display();
    }
    return 0;
}
