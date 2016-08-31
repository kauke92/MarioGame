#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include <sstream>
#include <iostream>
#include "iofile.h"
#include <QPainter>
#include <QString>


class Object {
public:

    Object(int count = 0) : m_x(0), m_y(0), m_v(0), m_count(count) {

	}

	void setX(int x);
	void setWidth(int x);
    void setHeight(int y);
	void setY(int y);
	void setV(int v);
    void setCount(int count);
    void setSpacing(int count);
	void setFile(std::string file);
	void setSize(std::string size);
    void setShape(std::string shape);
    void setColor(std::string color);
    void setErrorMsg(std::string error);

	void overrideV(int v);
	void overrideX(int x);
	void overrideY(int y);

	bool collisionChecker(Object* o, int velocity);
	std::string SetIntFromFile(std::string section, std::string key, int *value);


	int getX();
	int getY();
    int getV();
    int getWidth();
    int getHeight();
    int getCount();
    int getSpacing();
    class State* state;
	std::string getSize();
	std::string getFile();
    std::string getShape();
    std::string getColor();
    std::string getErrorMsg();

    void notify(class Dialog& dialog);
    void update(class Dialog& dialog);

    void setState(State* state);
    void nextState() {

    }



private:
    int m_x;
    int m_y;
    int m_v;
    int m_width;
    int m_height;
    int m_count;
    int m_spacing;
    std::string m_file;
	std::string m_size;
    std::string m_shape;
    std::string m_color;
    std::string m_errMsg;
};

class ObjectBuilder{
public:
    Object* getObject();
	void createNewObject(int count = 0);
    virtual void buildProperties(IOFile io) = 0;
   protected:
    Object* m_object;
};


class StickmanBuilder : public ObjectBuilder {
public:
    virtual void buildProperties(IOFile io);
};

class BackgroundBuilder : public ObjectBuilder {
public:
	virtual void buildProperties(IOFile io);
};

class ObstacleBuilder : public ObjectBuilder {
	public:
		virtual void buildProperties(IOFile io);
};

class StageBuilder {
public:
	Object* constructObject(ObjectBuilder* ob, IOFile io, int count = 0);
private:
    ObjectBuilder* m_objectBuilder;
};

class State
{
public:
    State(int);
    virtual ~State() {

    }

    int stateIndex;
    int jumpHeight;
    double gravity;
    bool destroyObstabcle;
    virtual void nextState(Object& stickman) = 0;
    virtual void prevState(Object& stickman) = 0;

};

class TinyMode: public State
{
public:
    TinyMode(int stateIndex, int jumpHeight, double gravity)
        : State(stateIndex) {
        this->jumpHeight = jumpHeight - 8;
        this->gravity = gravity - 0.5;
        this->destroyObstabcle = false;
    }

    ~TinyMode() {

    }

    void nextState(Object& stickman);
    void prevState(Object& stickman);


};
class NormalMode: public State
{
public:
    NormalMode(int stateIndex, int jumpHeight, double gravity)
        : State(stateIndex) {
        this->jumpHeight = jumpHeight - 5 ;
        this->gravity = gravity - 0.3;
        this->destroyObstabcle = false;
    }
    ~NormalMode() {

    }

    void nextState(Object& stickman);
    void prevState(Object& stickman);
};
class LargeMode: public State
{
public:
    LargeMode(int stateIndex, int jumpHeight, double gravity)
        : State(stateIndex) {
        this->jumpHeight = jumpHeight - 3;
        this->gravity = gravity - 0.2;
        this->destroyObstabcle = false;
    }
    ~LargeMode() {

    }

    void nextState(Object& stickman);
    void prevState(Object& stickman);
};
class GiantMode: public State
{
public:
    GiantMode(int stateIndex, int jumpHeight, double gravity)
        : State(stateIndex) {
        this->jumpHeight = jumpHeight - 0;
        this->gravity = gravity - 0.0;
        this->destroyObstabcle = true;
    }
    ~GiantMode() {

    }

    void nextState(Object& stickman);
    void prevState(Object& stickman);
};

#endif // OBJECT_H
