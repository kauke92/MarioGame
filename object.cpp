#include "object.h"
State::State(int stateIndex)
{
    this->stateIndex = stateIndex;
}

void TinyMode::nextState(Object& stickman) {

    stickman.setState(new NormalMode((this->stateIndex  +  1) % 4, this->jumpHeight  + 8,
                                     this->gravity   + 0.5));
}
void NormalMode::nextState(Object& stickman) {
    stickman.setState(new LargeMode((this->stateIndex  +  1) % 4, this->jumpHeight  + 5,
                                     this->gravity + 0.3));
}
void LargeMode::nextState(Object& stickman) {
    stickman.setState(new GiantMode((this->stateIndex  +  1) % 4, this->jumpHeight  + 3,
                                     this->gravity + 0.2));
}
void GiantMode::nextState(Object& stickman) {
    //cannot upgrage the GrantMode
}

void TinyMode::prevState(Object& stickman) {
    //cannot downgrade the TinyMode
}
void NormalMode::prevState(Object& stickman) {
    stickman.setState(new TinyMode((this->stateIndex - 1) % 4, this->jumpHeight  + 5,
                                     this->gravity + 0.3));
}
void LargeMode::prevState(Object& stickman) {
    stickman.setState(new NormalMode((this->stateIndex - 1) % 4, this->jumpHeight  + 3,
                                     this->gravity + 0.2));
}
void GiantMode::prevState(Object& stickman) {
    stickman.setState(new LargeMode((this->stateIndex - 1) % 4, this->jumpHeight  + 0,
                                     this->gravity + 0.0));
}

void Object::setState(State* state) {
    if (this->state) {
        delete this->state;
    }
    this->state = state;
}

void Object::setWidth(int x) {
    m_width = x;

}

void Object::setHeight(int y) {
    m_height = y;

}

int Object::getHeight() {
    return m_height;
}

int Object::getWidth() {
    return m_width;
}

void Object::setFile(std::string file) {
    m_file = file;
}

void Object::setV(int v) {
    m_v = m_v  +  v;
}

void Object::setX(int x) {
    m_x = m_x  +  x;
}

void Object::setY(int y) {
    m_y = m_y  +  y;
}

int Object::getV() {
    return m_v;
}

int Object::getX() {
    return m_x;
}

int Object::getY() {
    return m_y;
}

std::string Object::getFile() {
    return m_file;
}

std::string Object::getSize() {
    return m_size;
}

void Object::setSize(std::string size) {
    m_size = size;
}

Object* ObjectBuilder::getObject() {
    return m_object;
}

void ObjectBuilder::createNewObject(int count) {
    m_object = new Object(count);
}

// START ADDED

void Object::setCount(int count) {
    m_count = count;
}

int Object::getCount() {
	return m_count;
}

void Object::setSpacing(int count) {
    m_spacing = count;
}

int Object::getSpacing() {
	return m_spacing;
}

void Object::overrideV(int v) {
    m_v = v;
}

void Object::overrideX(int x) {
    m_x = x;
}

void Object::overrideY(int y) {
    m_y = y;
}

std::string Object::getShape() {
	return m_shape;
}

void Object::setShape(std::string shape) {
    m_shape = shape;
}

std::string Object::getColor() {
	return m_color;
}

void Object::setColor(std::string color) {
    m_color = color;
}

std::string Object::getErrorMsg() {
	return m_errMsg;
}

void Object::setErrorMsg(std::string error) {
    m_errMsg = m_errMsg  +  error;
}


bool Object::collisionChecker(Object* o, int velocity) {

	// Check for collision in next frame, thus given object's X-Position is temporarily moved.
	o->setX(velocity);

	// Checking for collision if given object is larger than m_object, checking collisions on both the X-Axis and Y-Axis
    if (((o->getX() >= m_x) && (o->getX() <= m_x  +  m_width))
            || ((o->getX()  +  o->getWidth() >= m_x) && (o->getX()  +  o->getWidth() <= m_x  +  m_width))) {

        if (((o->getY() >= m_y) && (o->getY() <= m_y  +  m_height))
                || ((o->getY()  +  o->getHeight() >= m_y) && (o->getY()  +  o->getHeight() <= m_y  +  m_height))) {
			o->setX(-velocity);
			return true;
		}
	}

	// Checking for collision if given object is smaller than m_object, checking collisions on both the X-Axis and Y-Axis
    if(((m_x >= o->getX()) && (m_x <= o->getX()  +  o->getWidth()))
            || ((m_x  +  m_width >= o->getX()) && (m_x  +  m_width <= o->getX()  +  o->getWidth()))) {

        if (((m_y >= o->getY()) && (m_y <= o->getY()  +  o->getHeight()))
                || ((m_y  +  m_height >= o->getY()) && (m_y  +  m_height <= o->getY()  +  o->getHeight()))) {
			o->setX(-velocity);
			return true;
		}
	}

	o->setX(-velocity);
	return false;
}

std::string Object::SetIntFromFile(std::string section, std::string key, int *value) {

	// Attempts to retrieve int from file using the specified ini section and key.
	// Captures and returns error message if retrieval is unsuccessful.

	IOFile io;
    std::string strToCheck = io.getElement("["  +  section  +  "]", key  +  "=");

    if (strToCheck == "nothing found") {
        return "\n- The input for ["  +  section  +  "]->"  +  key  +  " is missing.";
	}

    long long intAsLong = atoi(strToCheck.c_str());
    std::stringstream ss;
    ss << intAsLong ;
    std::string parsedInt =ss.str();

    if ((isdigit(strToCheck[0]) == 0 && strToCheck[0] != '-') || (strToCheck.length() > parsedInt.length())) {
        return "\n- The input of '"  +  strToCheck  +  "' for ["  +  section  +  "]->"  +  key  +  " is not valid.";
	}

	if (atoi(strToCheck.c_str()) < 0) {
        return "\n- The input of '"  +  strToCheck  +  "' for ["  +  section  +  "]->"  +  key  +  " is not valid.";
	}

    *value = atoi(strToCheck.c_str());

	return "";
}

void ObstacleBuilder::buildProperties(IOFile io) {

	// All object properties are retrieved from the config file, and returns an error if occurred.
	// The int values use the SetIntFromFile() method, while the string values are checked against
	// a list of expected values.

	std::ostringstream oss;
	oss << "Obstacle" << m_object->getCount();
    int parsedVal = 0;
    std::string givenStr = "";

	m_object->setErrorMsg(m_object->SetIntFromFile("Stickman", "Velocity", &parsedVal));
	m_object->setV(parsedVal);

	m_object->setErrorMsg(m_object->SetIntFromFile("Background", "WindowWidth", &parsedVal));
	m_object->setX(parsedVal);

	m_object->setErrorMsg(m_object->SetIntFromFile(oss.str(), "StartY", &parsedVal));
	m_object->setY(parsedVal);

    givenStr = io.getElement("["  +  oss.str()  +  "]", "Shape=");
	m_object->setShape(givenStr);
    if (m_object->getShape() == "nothing found") {
        m_object->setErrorMsg("\n- The input for ["  +  oss.str()  +  "]->Shape is missing.");
    } else if ((m_object->getShape() != "triangle") && (m_object->getShape() != "circle")
               && (m_object->getShape() != "rectangle")) {
        m_object->setErrorMsg("\n- The input of '"  +  givenStr  +  "' for ["  +  oss.str()  +  "]->Shape is not valid.");
	}

    givenStr = io.getElement("["  +  oss.str()  +  "]", "Color=");
	m_object->setColor(givenStr);
    if (m_object->getColor() == "nothing found") {
        m_object->setErrorMsg("\n- The input for ["  +  oss.str()  +  "]->Color is missing.");
    } else if ((m_object->getColor() != "red") && (m_object->getColor() != "yellow")
               && (m_object->getColor() != "white") && (m_object->getColor() != "green")) {
        m_object->setErrorMsg("\n- The input of '"  +  givenStr  +  "' for ["  +  oss.str()  +  "]->Color is not valid.");
	}

	m_object->setErrorMsg(m_object->SetIntFromFile(oss.str(), "Spacing", &parsedVal));
	m_object->setSpacing(parsedVal);

	m_object->setErrorMsg(m_object->SetIntFromFile(oss.str(), "Width", &parsedVal));
	m_object->setWidth(parsedVal);

	m_object->setErrorMsg(m_object->SetIntFromFile(oss.str(), "Height", &parsedVal));
	m_object->setHeight(parsedVal);

    m_object->setFile("obstacle_"  +  m_object->getShape()  +  "_"  +  m_object->getColor()  +  ".png");

}
// END ADDED

// UPDATED
void StickmanBuilder::buildProperties(IOFile io) {
    int parsedVal = 0;
    std::string givenStr = "";

	//Setting object properties.

	// Added: Setting from config now returns errors if input is invalid or missing.
	m_object->setFile("stickman-306967_640.png");

	m_object->setErrorMsg(m_object->SetIntFromFile("Stickman", "Velocity", &parsedVal));
	m_object->setV(parsedVal);

	m_object->setErrorMsg(m_object->SetIntFromFile("Stickman", "StartX", &parsedVal));
	m_object->setX(parsedVal);

	m_object->setErrorMsg(m_object->SetIntFromFile("Stickman", "StartY", &parsedVal));
	m_object->setY(parsedVal);

    givenStr = io.getElement("[Stickman]","Size=");
	m_object->setSize(givenStr);
    if (m_object->getSize() == "nothing found") {
		m_object->setErrorMsg("\n- The input for [Stickman]->Size is missing.");
    } else if ((m_object->getSize() != "tiny") && (m_object->getSize() != "normal")
               && (m_object->getSize() != "large") && (m_object->getSize() != "giant")) {
        m_object->setErrorMsg("\n- The input of '"  +  givenStr  +  "' for [Stickman]->Size is not valid.");
	}

	//Setting object size properties.
    if (m_object->getSize() == "tiny") {
		m_object->setHeight(50);
        m_object->setWidth(50);
    } else if(m_object->getSize() == "normal") {
        m_object->setHeight(80);
        m_object->setWidth(80);
    } else if(m_object->getSize() == "large") {
        m_object->setHeight(100);
        m_object->setWidth(100);
    } else if(m_object->getSize() == "giant") {
        m_object->setHeight(150);
        m_object->setWidth(150);
	}
    m_object->state = NULL;

}


void BackgroundBuilder::buildProperties(IOFile io) {

	//Setting background properties.
    m_object->setFile(io.getElement("[Background]","Image="));

    if (m_object->getFile() == "nothing found") {
		m_object->setErrorMsg("\n- The input for [Background]->Image is missing.");
    } else if ((m_object->getFile() != "background.jpg")) {
        m_object->setErrorMsg("\n- The input of '"  +  io.getElement("[Background]","Image=")
                               +  "' for [Background]->Image is not valid.");
	}
}


Object* StageBuilder::constructObject(ObjectBuilder *ob, IOFile io, int count) {
    m_objectBuilder = ob;
	m_objectBuilder->createNewObject(count);
	m_objectBuilder->buildProperties(io);
	return m_objectBuilder->getObject();
}
