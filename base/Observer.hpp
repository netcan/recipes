class Observable;
class Observer {
    public:
        virtual void Update() = 0;
        void Observe(Observable* s);
        virtual ~Observer() {}
    private:
};

class Observable {
    public:
        void Register(Observer* obs);
        void UnRegister(Observer* obs);
        void NotifyObservers();
    private:
};
