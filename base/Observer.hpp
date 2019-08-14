/* author: netcan
 * date: 2019/07/29 22:45
 * @shanghai
 */
#pragma once

namespace base {
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

}
