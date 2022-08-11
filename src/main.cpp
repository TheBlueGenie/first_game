#include <SFML/Graphics.hpp>

// Constants for window size
constexpr int windowWidth{ 800 }, windowHeight{ 600 }, fps{ 60 };

// Constants for ball
constexpr float ballRadius{ 10.f }, ballVelocity{ 8.f };

// Constants for paddle
constexpr float paddleWidth{ 60.f }, paddleHeight{ 20.f }, paddleVelocity{ 6.f };

// Constants for blocks
constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };


// Structure of the ball object
struct Ball {
  sf::CircleShape shape;

  sf::Vector2f velocity{-ballVelocity, -ballVelocity};

  Ball(float mx, float my) {
    shape.setPosition(mx, my);
    shape.setRadius(ballRadius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(ballRadius, ballRadius);
  }

  void update() { 
    shape.move(velocity); 

    if (left() < 0){
      velocity.x = ballVelocity;
    }
    else if (right() > windowWidth) {
      velocity.x = -ballVelocity;
    }
    else if (top() < 0) {
      velocity.y = ballVelocity;
    }
    else if (bottom() > windowHeight) {
      velocity.y = -ballVelocity;
    }
  }

  // Property methods
  float x()     { return shape.getPosition().x; }
  float y()     { return shape.getPosition().y; }
  float left()  { return x() - shape.getRadius(); }
  float right() { return x() + shape.getRadius(); }
  float top()   { return y() - shape.getRadius(); }
  float bottom(){ return y() + shape.getRadius(); }
};

struct Paddle {
  sf::RectangleShape shape;
  sf::Vector2f velocity;

  Paddle(float mX, float mY) {
    shape.setPosition(mX, mY);
    shape.setSize({paddleWidth, paddleHeight});
    shape.setFillColor(sf::Color::Blue);
    shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
  }

  void update() { 
    shape.move(velocity); 

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && 
        left() > 0) {
      velocity.x = -paddleVelocity;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
        right() < windowWidth) {
      velocity.x = paddleVelocity;
    }
    else {
      velocity.x = 0;
    }
  }

  // Property methods
  float x()     { return shape.getPosition().x; }
  float y()     { return shape.getPosition().y; }
  float left()  { return x() - shape.getSize().x / 2.f; }
  float right() { return x() + shape.getSize().x / 2.f; }
  float top()   { return y() - shape.getSize().y / 2.f; }
  float bottom(){ return y() + shape.getSize().y / 2.f; }
};

struct Brick {
  sf::RectangleShape shape;
  bool destroyed{false};

  Brick(float mX, float mY) {
    shape.setPosition(mX, mY);
    shape.setSize({blockWidth, blockHeight});
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
  }

  // Property methods
  float x()     { return shape.getPosition().x; }
  float y()     { return shape.getPosition().y; }
  float left()  { return x() - shape.getSize().x / 2.f; }
  float right() { return x() + shape.getSize().x / 2.f; }
  float top()   { return y() - shape.getSize().y / 2.f; }
  float bottom(){ return y() + shape.getSize().y / 2.f; }
};


template<class T1, class T2> bool isIntersecting(T1& mA, T2& mB) {
  return mA.right() >= mB.left() &&
          mA.left() <= mB.right() &&
          mA.bottom() >= mB.top() &&
          mA.top() <= mB.bottom();
}

void testCollision(Paddle& mPaddle, Ball& mBall) {
  if (!isIntersecting(mPaddle, mBall)) return;

  mBall.velocity.y = -ballVelocity;

  if (mBall.x() < mPaddle.x())
    mBall.velocity.x = -ballVelocity;
  else
    mBall.velocity.x = ballVelocity;
}

void testCollision(Brick& mBrick, Ball& mBall) {
  if (!isIntersecting(mBrick, mBall)) return;

  mBrick.destroyed = true;

  float overlapLeft{mBall.right() - mBrick.right()};
  float overlapRight{mBrick.right() - mBall.left()};
  float overlapTop{mBall.bottom() - mBrick.top()};
  float overlapBottom{mBrick.top() - mBall.top()};

  bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));

  bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

  bool minOverlapX{ballFromLeft ? overlapLeft : overlapRight};

  bool minOverlapY{ballFromTop ? overlapTop : overlapBottom};

  if (abs(minOverlapX) < abs(minOverlapY))
    mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
  else
    mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;


}

int main()
{
  sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Breakout");
  window.setFramerateLimit(fps);

  // Create the ball and position it at screen center;
  Ball ball{windowWidth / 2, windowHeight / 2};

  // Create the paddle
  Paddle paddle{windowWidth / 2, windowHeight - paddleHeight};

  // Create a vector to store all the blocks
  std::vector<Brick> bricks;

  // fill the vector with a 2d for loop
  for (int iX = 0; iX < countBlocksX; ++iX) {
    for (int iY = 0; iY < countBlocksY; ++iY) {
      bricks.emplace_back((iX + 1) * (blockWidth + 3) + 22, 
                          (iY + 2) * (blockHeight +3));
    }
  }
  

  // Game Loop
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
      window.close();
    }

    // ---------------------------------------------------------------------------------------
    // Update 
      ball.update();
      paddle.update();

      testCollision(paddle, ball);

      for (auto& brick:bricks) testCollision(brick, ball);

    // ---------------------------------------------------------------------------------------
    // Draw

      // Clear the window
      window.clear();

      // Draw to the window
      window.draw(ball.shape);
      window.draw(paddle.shape);

      // Draw all the bricks
      for( auto& brick:bricks ) window.draw(brick.shape);

      // Remove destroyed blocks using an STL Algorithm
      bricks.erase(remove_if(begin(bricks), end(bricks),
        [](const Brick& mBrick) { return mBrick.destroyed; }),
        end(bricks));

      // display the window contents
      window.display();
  }

  return 0;
}
