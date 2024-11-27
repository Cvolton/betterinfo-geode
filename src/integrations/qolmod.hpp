#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace QOLModExt
{
    enum class ModuleExtType
    {
        Boolean,
    };

    class ModuleExt
    {
        private:
            std::string name;
            std::string id;
            std::string description;

            bool canBeToggled = true;
            std::string notTogglableWarning;

            bool enabledByDefault;
            bool enabled;

            std::function<void(bool)> onToggle;

            ModuleExtType type = ModuleExtType::Boolean;

        public:
            ModuleExt(std::string id)
            {
                this->id = id;
                name = id;
                description = "This is the help text";
            }

            void setName(std::string name)
            {
                this->name = name;
            }

            std::string getName()
            {
                return name;
            }

            std::string getID()
            {
                return id;
            }

            void setDescription(std::string description)
            {
                this->description = description;
            }

            std::string getDescription()
            {
                return description;
            }

            void setEnabledByDefault(bool enabled)
            {
                this->enabledByDefault = enabled;
            }

            bool getEnabledByDefault()
            {
                return enabledByDefault;
            }

            void setCanBeToggled(bool can, std::string message)
            {
                this->canBeToggled = can;
                this->notTogglableWarning = message;
            }

            bool getCanBeToggled()
            {
                return canBeToggled;
            }

            std::string getCantBeToggledWarning()
            {
                return notTogglableWarning;
            }

            void setOnToggle(std::function<void(bool)> onToggle)
            {
                this->onToggle = onToggle;
            }

            std::function<void(bool)> getOnToggle()
            {
                return onToggle;
            }

            void setType(ModuleExtType type)
            {
                this->type = type;
            }

            ModuleExtType getType()
            {
                return type;
            }

            void setEnabled(bool enabled)
            {
                this->enabled = enabled;
            }

            bool getEnabled()
            {
                return enabled;
            }
    };

    class WindowExt
    {
        private:
            std::string name;
            std::string id;
            int priority;
            std::vector<ModuleExt*> modules;
            std::function<void(CCMenu*, ScrollLayer*)> createPostHook;
        
        public:
            // Please don't use this
            WindowExt(std::string id)
            {
                this->id = id;
                this->name = id;
            }

            std::string getID()
            {
                return id;
            }

            void setName(std::string name)
            {
                this->name = name;
            }

            std::string getName()
            {
                return name;
            }

            void setPriority(int priority)
            {
                this->priority = priority;
            }

            int getPriority()
            {
                return priority;
            }

            void addModule(ModuleExt* module)
            {
                modules.push_back(module);
            }

            std::vector<ModuleExt*> getModules()
            {
                return modules;
            }

            // Runs after the menu is created, for adding your own nodes
            void setPostCreateHook(std::function<void(CCMenu*, ScrollLayer*)> hook)
            {
                this->createPostHook = hook;
            }
    };

    class PushWindowEvent : public Event
    {
        public:
            WindowExt* window;

            PushWindowEvent(WindowExt* window)
            {
                this->window = window;
            }
    };

    class ToggleModuleEvent : public Event
    {
        public:
            std::string id;
            bool enabled;

            ToggleModuleEvent(std::string id, bool enabled)
            {
                this->id = id;
                this->enabled = enabled;
            }
    };

    /// @brief Creates a window and returns it
    /// @param id An id for the window, this is not shown to the user and is currently not used internally. This may change, please do not change the ID
    /// @return WindowExt*
    inline WindowExt* createWindow(std::string id)
    {
        auto window = new WindowExt(id);

        return window;
    }

    inline ModuleExt* createModule(std::string id)
    {
        auto mod = new ModuleExt(id);

        return mod;
    }

    /// @brief Adds a window and its modules to the UI.
    /// @param window 
    inline void pushWindow(WindowExt* window)
    {
        auto e = PushWindowEvent(window);
        e.post();
    }

    inline void setModuleEnabled(std::string moduleID, bool enabled)
    {
        auto e = ToggleModuleEvent(moduleID, enabled);
        e.post();
    }
};