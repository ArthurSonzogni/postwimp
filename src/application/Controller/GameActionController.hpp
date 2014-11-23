#ifndef GAMEACTIONCONTROLLER_2FUBJ7UE
#define GAMEACTIONCONTROLLER_2FUBJ7UE


class GameAction;
class Application;


class GameActionController
{
    public:
        virtual void update(GameAction& gameAction, Application& application) = 0;
};

#endif /* end of include guard: GAMEACTIONCONTROLLER_2FUBJ7UE */
