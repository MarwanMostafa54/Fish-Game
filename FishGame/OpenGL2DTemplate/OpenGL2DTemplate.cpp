#include <math.h>
#include <vector>

#include <cstdlib>
#include <glut.h>
#ifndef M_PI  
#define M_PI 3.14159265358979323846  
#endif

// Bubble structure
struct Bubble {
    float x, y;
    float radius;
    float speed;
};
struct Mine {
    float x, y;
    float radius;
    float speedX, speedY;
};
struct Gill {
    float x, y;
};
struct Rock {
    float x, y;
    float radius;
};


// Global variables
std::vector<Bubble> bubbles;
std::vector<Mine> mines;
std::vector<Gill> gills;
std::vector<Rock> rocks;
const int numRocks = 5;
const int numBubbles = 10;
const int numMines = 5;
float sharkX = 150.0f, sharkY = 150.0f; // Initial shark position
float mouseX = 150.0f, mouseY = 150.0f; // Mouse position
float gillX = 0.0f, gillY = 0.0f;

// Function prototypes
void checkCollisionWithRocks();

void Display();
void drawOceanGradient();
void drawSeafloor();
void drawBubbles();
void updateBubbles(int value);
void drawShark();
void onMouseMove(int x, int y);
void drawGill();
void renderText(float x, float y, const char* text, float r, float g, float b);
bool shouldRenderText(int collisionTime);
bool collisionDetected = false;
int collisionTime = 0;
bool mineCollisionDetected = false;
int mineCollisionTime = 0;

void initRocks() {
    for (int i = 0; i < numRocks; ++i) {
        Rock rock;
        rock.x = rand() % 300;
        rock.y = 50.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 7.0f)); // Random y between 50 and 57
        rock.radius = 15.0f; // Set a fixed radius for rocks
        rocks.push_back(rock);
    }
}

// Function to draw a rock with a more realistic shape
void drawRock(float x, float y, float radius) {
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color for the rock

    // Draw the main body of the rock using a polygon with irregular vertices
    glBegin(GL_POLYGON);
    for (int i = 0; i < 8; ++i) { // Use 8 vertices for an irregular shape
        float angle = i * M_PI / 4; // Divide the circle into 8 parts
        float offset = (rand() % 5) / 10.0f; // Random offset for irregularity
        glVertex2f(x + (radius + offset) * cos(angle), y + (radius + offset) * sin(angle));
    }
    glEnd();

    // Draw lines to add texture to the rock
    glColor3f(0.3f, 0.3f, 0.3f); // Darker gray for the lines
    glLineWidth(2.0f); // Set line thickness
    glBegin(GL_LINES);
    for (int i = 0; i < 8; ++i) {
        float angle = i * M_PI / 4;
        float offset = (rand() % 5) / 10.0f;
        glVertex2f(x, y); // Start from the center
        glVertex2f(x + (radius + offset) * cos(angle), y + (radius + offset) * sin(angle)); // End at the edge
    }
    glEnd();
}

// Function to initialize bubbles
void initBubbles() {
    for (int i = 0; i < numBubbles; ++i) {
        Bubble bubble;
        bubble.x = rand() % 300;
        bubble.y = rand() % 300;
        bubble.radius = 2.0f + (rand() % 3); // Random radius between 2 and 5
        bubble.speed = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.5f)); // Random speed between 0.5 and 2.0
        bubbles.push_back(bubble);
    }
}
// Function to initialize mines
void initMines() {
    for (int i = 0; i < numMines; ++i) {
        Mine mine;
        mine.x = rand() % 300;
        mine.y = rand() % 300;
        mine.radius = 10.0f;
        mine.speedX = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f)); // Random speed between 0.1 and 0.6
        mine.speedY = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f)); // Random speed between 0.1 and 0.6
        mines.push_back(mine);
    }
}

void drawGill() {
    for (const auto& gill : gills) {
        glPushMatrix(); // Save the current transformation matrix
        glTranslatef(gill.x, gill.y, 0.0f); // Position Gill in the scene

        // Body (larger diamond shape)
        glColor3f(1.0f, 0.5f, 0.0f); // Orange color for the body
        glBegin(GL_POLYGON);
        glVertex2f(0.0f, 7.5f); // Top point
        glVertex2f(5.625f, 0.0f); // Right point
        glVertex2f(0.0f, -7.5f); // Bottom point
        glVertex2f(-5.625f, 0.0f); // Left point
        glEnd();

        // Stripes
        glColor3f(1.0f, 1.0f, 1.0f); // White color for the stripes
        glLineWidth(2.0f); // Set line thickness
        glBegin(GL_LINES);
        float stripeSpacing = 2.0f; // Increase distance between stripes
        for (float x = -4.0f; x <= 4.0f; x += stripeSpacing) {
            float yTop = 7.5f - fabs(x) * 1.5f; // Calculate top y-coordinate based on x
            float yBottom = -7.5f + fabs(x) * 1.5f; // Calculate bottom y-coordinate based on x
            glVertex2f(x, yTop);
            glVertex2f(x, yBottom);
        }
        glEnd();

        // Eye
        glColor3f(1.0f, 1.0f, 1.0f); // White color for the eye
        glBegin(GL_POLYGON);
        float r = 1.0f; // Radius of the eye
        for (int i = 0; i < 360; i++) {
            float deginrad = i * M_PI / 180;
            glVertex2f(-3.75f + r * cos(deginrad), 1.875f + r * sin(deginrad));
        }
        glEnd();

        // Pupil
        glColor3f(0.0f, 0.0f, 0.0f); // Black color for the pupil
        r = 0.3f; // Radius of the pupil
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float deginrad = i * M_PI / 180;
            glVertex2f(-3.75f + r * cos(deginrad), 1.875f + r * sin(deginrad));
        }
        glEnd();

        // Tail
        glColor3f(1.0f, 0.5f, 0.0f); // Orange color for the tail
        glBegin(GL_TRIANGLES);
        glVertex2f(5.625f, 0.0f); // Tail base
        glVertex2f(7.5f, 1.875f); // Tail top point
        glVertex2f(7.5f, -1.875f); // Tail bottom point
        glEnd();

        glPopMatrix(); // Restore the previous transformation matrix
    }
}


void drawMine(float x, float y, float radius) {
    // Draw the body of the mine
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for the mine body
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float deginrad = i * M_PI / 180; // Convert degrees to radians
        glVertex2f(x + radius * cos(deginrad), y + radius * sin(deginrad)); // Circle vertices
    }
    glEnd();

    // Add a lighter shade for a gradient effect
    glColor3f(0.4f, 0.4f, 0.4f); // Lighter gray for the top part
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float deginrad = i * M_PI / 180; // Convert degrees to radians
        float newRadius = radius * 0.85f; // Slightly smaller radius for the top
        glVertex2f(x + newRadius * cos(deginrad), y + newRadius * sin(deginrad)); // Circle vertices
    }
    glEnd();

    // Draw the lines extending from the mine
    glColor3f(0.0f, 0.0f, 0.0f); // Dark gray color for lines
    glLineWidth(2.5f); // Set line thickness

    for (int i = 0; i < 8; i++) {
        float angle = (float)i / 8 * 2.0f * M_PI; // Angle for each line
        float x1 = x + cos(angle) * (radius + 5.0f); // End point x-coordinate
        float y1 = y + sin(angle) * (radius + 5.0f); // End point y-coordinate

        // Draw the line from the center to the outer point
        glBegin(GL_LINES);
        glVertex2f(x, y); // Start of the line (center)
        glVertex2f(x1, y1); // End of the line (outer point)
        glEnd();
    }

    // Optionally draw a central "detonator" part
    glColor3f(0.8f, 0.2f, 0.2f); // Red color for the detonator
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float deginrad = i * M_PI / 180; // Convert degrees to radians
        glVertex2f(x + (radius * 0.2f) * cos(deginrad), y + (radius * 0.2f) * sin(deginrad)); // Smaller circle in the center
    }
    glEnd();
}

void addGill(int value) {
    Gill newGill;
    newGill.x = rand() % 300;
    newGill.y = rand() % 300;
    gills.push_back(newGill);
    glutPostRedisplay();
    glutTimerFunc(3000, addGill, 0); // Call addGill every 3 seconds
}



// Function to display the scene
bool shouldRenderText(int collisionTime) {
    return (glutGet(GLUT_ELAPSED_TIME) - collisionTime) < 500;
}
void drawSeaweed(float x, float y, float height, float time) {
    glColor3f(0.0f, 0.5f, 0.0f); // Green color for the seaweed

    glBegin(GL_LINE_STRIP);
    glVertex2f(x, y); // Start point at the base

    // Draw the seaweed using a sine wave for left and right movement
    for (int i = 0; i <= 10; ++i) {
        float t = i / 10.0f;
        float sway = sin(time + t * M_PI * 0.5f) * 5.0f; // Swaying effect with slower frequency and reduced amplitude
        float xt = x + sway;
        float yt = y + t * height;
        glVertex2f(xt, yt);
    }

    glEnd();
}




void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background layers
    drawOceanGradient();  // Ocean gradient background
    drawSeafloor();
    drawShark();       
    drawBubbles();        // Bubbles moving upward
    drawGill();           // Gill from Finding Nemo
    for (const auto& mine : mines) {
        drawMine(mine.x, mine.y, mine.radius);
    }
    for (const auto& rock : rocks) {
        drawRock(rock.x, rock.y, rock.radius);
    }
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Time in seconds

    for (int i = 0; i < 10; ++i) {
        float x = 20.0f + i * 30.0f; // Position seaweed at intervals
        float y = 50.0f; // Base y position
        float height = 100.0f; // Random height between 100 and 150
        drawSeaweed(x, y, height, time);
    }
    checkCollisionWithRocks();

    if (collisionDetected && shouldRenderText(collisionTime)) {
        renderText(100.0f, 150.0f, "You Win!", 0.0f, 1.0f, 0.0f); // Green "You Win" message
    }

    if (mineCollisionDetected && shouldRenderText(mineCollisionTime)) {
        renderText(100.0f, 150.0f, "You Lose!", 1.0f, 0.0f, 0.0f); // Red "You Lose" message
    }

    glFlush();
}
bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (r1 + r2);
}

// Function to check collision with mines and prevent the shark from moving through them
// Function to check collision with rocks and prevent the shark from moving through them
void checkCollisionWithRocks() {
    float mouthOffsetX = -10.0f;
    float mouthOffsetY = 0.0f;
    float mouthOffsetX2 = 10.0f;
    float mouthOffsetY2 = 0.0f;
    float mouthOffsetX3 = 0.0f;
    float mouthOffsetY3 = -4.0f;
    float mouthOffsetX4 = 0.0f;
    float mouthOffsetY4 = 15.0f;
    float mouthX = sharkX + mouthOffsetX;
    float mouthY = sharkY + mouthOffsetY;
    float mouthX2 = sharkX + mouthOffsetX2;
    float mouthY2 = sharkY + mouthOffsetY2;
    float mouthX3 = sharkX + mouthOffsetX3;
    float mouthY3 = sharkY + mouthOffsetY3;
    float mouthX4 = sharkX + mouthOffsetX4;
    float mouthY4 = sharkY + mouthOffsetY4;

    for (const auto& rock : rocks) {
        float dxMouth = rock.x - mouthX;
        float dyMouth = rock.y - mouthY;
        float dxMouth2 = rock.x - mouthX2;
        float dyMouth2 = rock.y - mouthY2;
        float dxMouth3 = rock.x - mouthX3;
        float dyMouth3 = rock.y - mouthY3;
        float dxMouth4 = rock.x - mouthX4;
        float dyMouth4 = rock.y - mouthY4;

        float distanceMouth = sqrt(dxMouth * dxMouth + dyMouth * dyMouth);
        float distanceMouth2 = sqrt(dxMouth2 * dxMouth2 + dyMouth2 * dyMouth2);
        float distanceMouth3 = sqrt(dxMouth3 * dxMouth3 + dyMouth3 * dyMouth3);
        float distanceMouth4 = sqrt(dxMouth4 * dxMouth4 + dyMouth4 * dyMouth4);
        if (distanceMouth < rock.radius || distanceMouth2 < rock.radius || distanceMouth3 < rock.radius || distanceMouth4 < rock.radius) {
            

            // Prevent the shark from moving through the rock
            if (distanceMouth < rock.radius) {
                sharkX -= dxMouth * 0.5f; // Move the shark back along the x-axis
                sharkY -= dyMouth * 0.5f; // Move the shark back along the y-axis
            }
            if (distanceMouth2 < rock.radius) {
                sharkX -= dxMouth2 * 0.5f; // Move the shark back along the x-axis
                sharkY -= dyMouth2 * 0.5f; // Move the shark back along the y-axis
                sharkY -= dyMouth2 * 0.5f; // Move the shark back along the y-axis
            }
            if (distanceMouth3 < rock.radius) {
                sharkX -= dxMouth3 * 0.5f; // Move the shark back along the x-axis
                sharkY -= dyMouth3 * 0.5f; // Move the shark back along the y-axis
            }
            if (distanceMouth4 < rock.radius) {
                sharkX -= dxMouth4 * 1.0f; // Move the shark back along the x-axis
                sharkY -= dyMouth4 * 1.0f; // Move the shark back along the y-axis
            }
            break;
        }
    }
}



// Function to check collision with mines and prevent the shark from moving through them
void checkCollisionWithMines() {
    float mouthOffsetX = -10.0f;
    float mouthOffsetY = 0.0f;
    float mouthOffsetX2 = 10.0f;
    float mouthOffsetY2 = 0.0f;
    float mouthOffsetX3 = 0.0f;
    float mouthOffsetY3 = -4.0f;
    float mouthOffsetX4 = 0.0f;
    float mouthOffsetY4 = 15.0f;
    float mouthX = sharkX + mouthOffsetX;
    float mouthY = sharkY + mouthOffsetY;
    float mouthX2 = sharkX + mouthOffsetX2;
    float mouthY2 = sharkY + mouthOffsetY2;
    float mouthX3 = sharkX + mouthOffsetX3;
    float mouthY3 = sharkY + mouthOffsetY3;
    float mouthX4 = sharkX + mouthOffsetX4;
    float mouthY4 = sharkY + mouthOffsetY4;

    for (const auto& mine : mines) {
        float dxMouth = mine.x - mouthX;
        float dyMouth = mine.y - mouthY;
        float dxMouth2 = mine.x - mouthX2;
        float dyMouth2 = mine.y - mouthY2;
        float dxMouth3 = mine.x - mouthX3;
        float dyMouth3 = mine.y - mouthY3;
        float dxMouth4 = mine.x - mouthX4;
        float dyMouth4 = mine.y - mouthY4;

        float distanceMouth = sqrt(dxMouth * dxMouth + dyMouth * dyMouth);
        float distanceMouth2 = sqrt(dxMouth2 * dxMouth2 + dyMouth2 * dyMouth2);
        float distanceMouth3 = sqrt(dxMouth3 * dxMouth3 + dyMouth3 * dyMouth3);
        float distanceMouth4 = sqrt(dxMouth4 * dxMouth4 + dyMouth4 * dyMouth4);
        if (distanceMouth < mine.radius ||distanceMouth2 < mine.radius || distanceMouth3 < mine.radius || distanceMouth4 < mine.radius  ) {
            mineCollisionDetected = true;
            mineCollisionTime = glutGet(GLUT_ELAPSED_TIME);

            // Prevent the shark from moving through the mine
            if (distanceMouth < mine.radius) {
                sharkX -= dxMouth * 0.5f; // Move the shark back along the x-axis
                sharkY -= dyMouth * 0.5f; // Move the shark back along the y-axis
            }
            if (distanceMouth2 < mine.radius) {
                sharkX -= dxMouth2 * 0.5f; // Move the shark back along the x-axis
                sharkY -= dyMouth2 * 0.5f; // Move the shark back along the y-axis
            }
            if (distanceMouth3 < mine.radius) {
                sharkX -= dxMouth3 * 0.5f; // Move the shark back along the x-axis
                sharkY -= dyMouth3 * 0.5f; // Move the shark back along the y-axis
            }
            if (distanceMouth4 < mine.radius) {
                sharkX -= dxMouth4 * 1.0f; // Move the shark back along the x-axis
                sharkY -= dyMouth4 * 1.0f; // Move the shark back along the y-axis
            }
            break;
        }
    }
}




// Function to draw a realistic shark
void drawShark() {
    glPushMatrix(); // Save the current transformation matrix
    glTranslatef(sharkX, sharkY, 0.0f); // Move shark to its position

    glColor3f(0.4, 0.4, 0.4);

    // Body
    glBegin(GL_POLYGON);
    glVertex2f(-15, 0.0);
    glVertex2f(-10, 10);
    glVertex2f(10, 10);
    glVertex2f(10, 0.0);
    glEnd();

    // Tail part 1
    glBegin(GL_POLYGON);
    glVertex2f(10, 0.0);
    glVertex2f(10, 10);
    glVertex2f(15, 5);
    glEnd();

    // Tail part 2
    glBegin(GL_POLYGON);
    glVertex2f(15, 5);
    glVertex2f(16, 7);
    glVertex2f(16, 3);
    glEnd();

    // Top fin
    glBegin(GL_POLYGON);
    glVertex2f(5, 10);
    glVertex2f(7, 14);
    glVertex2f(7, 10);
    glEnd();

    // Bottom fin
    glBegin(GL_POLYGON);
    glVertex2f(6, -1);
    glVertex2f(4, 0);
    glVertex2f(6, 0);
    glEnd();

    // Mouth
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-5, 1);
    glVertex2f(5, 1);
    glVertex2f(5, 1);
    glVertex2f(6, 3);
    glEnd();

    // Gills
    glBegin(GL_LINES);
    glVertex2f(-9, 8);
    glVertex2f(-8, 7);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(5, 6);
    glVertex2f(6, 8);
    glEnd();

    // Eye
    float r = 1.5; // Adjust radius for visibility
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float deginrad = i * M_PI / 180; // Use M_PI
        glVertex2f(-5 + r * cos(deginrad), 7 + r * sin(deginrad));
    }
    glEnd();

    glPopMatrix(); // Restore the previous transformation matrix
}

void onMouseMove(int x, int y) {
    // Convert window coordinates to OpenGL coordinates
    mouseX = static_cast<float>(x) * 300.0f / 1200.0f;
    mouseY = 300.0f - static_cast<float>(y) * 300.0f / 700.0f;

    // Log the mouse position for debugging
  /*  printf("Mouse Position: (%f, %f)\n", mouseX, mouseY);*/
}


void updateGillPosition(int value) {
    gillX = rand() % 300;
    gillY = rand() % 300;
    glutPostRedisplay();
    glutTimerFunc(3000, updateGillPosition, 0); // Call updateGillPosition every 3 seconds
}



// Function to draw the ocean depth gradient
void drawOceanGradient() {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.3f, 0.6f);  // Light blue at the top
    glVertex2f(0.0f, 300.0f);
    glVertex2f(300.0f, 300.0f);

    glColor3f(0.0f, 0.1f, 0.3f);  // Darker blue at the bottom
    glVertex2f(300.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
}

// Function to draw a realistic seafloor
void drawSeafloor() {
    // Draw seafloor with smoother transitions
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 50.0f);
    glVertex2f(50.0f, 55.0f);
    glVertex2f(100.0f, 52.0f);
    glVertex2f(150.0f, 57.0f);
    glVertex2f(200.0f, 50.0f);
    glVertex2f(250.0f, 55.0f);
    glVertex2f(300.0f, 50.0f);
    glVertex2f(300.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();


}
void renderText(float x, float y, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);  // Set the text color
    glRasterPos2f(x, y); // Set the position for text
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text); // Render each character
        ++text;
    }
}

void drawBubbles() {
    glColor3f(1.0f, 1.0f, 1.0f);  // White bubbles
    for (const auto& bubble : bubbles) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float degInRad = i * M_PI / 180.0f;
            glVertex2f(bubble.x + bubble.radius * cos(degInRad), bubble.y + bubble.radius * sin(degInRad));
        }
        glEnd();
    }
}


void checkCollisionWithGill() {
    float mouthOffsetX = -10.0f;
    float mouthOffsetY = 0.0f;
    float mouthX = sharkX + mouthOffsetX;
    float mouthY = sharkY + mouthOffsetY;
    float mouthOffsetX2 = 10.0f;
    float mouthOffsetY2 = 0.0f;
    float mouthX2 = sharkX + mouthOffsetX2;
    float mouthY2 = sharkY + mouthOffsetY2;
    for (auto it = gills.begin(); it != gills.end(); ++it) {
        float gillDistance = sqrt(pow(mouthX - it->x, 2) + pow(mouthY - it->y, 2));
        float gillDistance2 = sqrt(pow(mouthX2 - it->x, 2) + pow(mouthY2 - it->y, 2));

        if (gillDistance < 10.0f|| gillDistance2 <10.0f) {
            collisionDetected = true;
            collisionTime = glutGet(GLUT_ELAPSED_TIME);
            gills.erase(it); // Remove the gill from the vector
            printf("Collision detected with Gill!\n");
            break;
        }
    }
}


// Function to update bubble positions
void updateBubbles(int value) {
    for (auto& bubble : bubbles) {
        bubble.y += bubble.speed;
        if (bubble.y > 300) {
            bubble.y = 0;
            bubble.x = rand() % 300;
        }
    }

    float dx = mouseX - sharkX;
    float dy = mouseY - sharkY;
    float distance = sqrt(dx * dx + dy * dy);
    float speed = 2.0f;
    if (distance > speed) {
        sharkX += dx / distance * speed;
        sharkY += dy / distance * speed;
    }

    for (auto& mine : mines) {
        mine.x += mine.speedX;
        mine.y += mine.speedY;
        if (mine.x > 300 || mine.x < 0) mine.speedX = -mine.speedX;
        if (mine.y > 300 || mine.y < 0) mine.speedY = -mine.speedY;
    }

    checkCollisionWithGill();
    checkCollisionWithMines();

    glutPostRedisplay();
    glutTimerFunc(16, updateBubbles, 0);
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(1200, 700);
    glutInitWindowPosition(150, 150);

    glutCreateWindow("OpenGL - 2D Ocean Background");
    glutDisplayFunc(Display);
    glutPassiveMotionFunc(onMouseMove); // Register mouse move callback

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, 300, 0.0, 300);

    initBubbles();
    initRocks(); // Initialize rocks
    initMines();
    glutTimerFunc(0, updateBubbles, 0); // Start the bubble update timer
    glutTimerFunc(0, addGill, 0); // Start the Gill addition timer

    glutMainLoop();
    return 0;
}
