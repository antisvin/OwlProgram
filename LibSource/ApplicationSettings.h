#ifndef __ApplicationSettingsH__
#define __ApplicationSettingsH__


/**
 * Controller for flash-based application settings storage
 */
class ApplicationSettings {
public:
    /**
     * Only a single write will be performed by this object by default. If
     * you'll need to write more than once, set multipleWrites = true when
     * initializing object.
     * 
     */
    ApplicationSettings(bool multipleWrites = false) : multipleWrites(multipleWrites) {};

    /**
     * Reset all values to defaults.
     * 
     * Note that new values will be used, but not stored automatically.
     * 
     * @return true if data was reset
     * 
     */
    bool reset();

    /**
     * Store updated values permanently in flash.
     * 
     * \warning Beware of excessive usage, flash memory has limited write resource.
     * 
     * @return true if data was stored
     * 
     */
    bool storeToFlash();

    /**
     * Load settings that are stored in flash
     * 
     * This method would have no visible effect unless some settings have been changed.
     * 
     * @return true if data was loaded
     */
    bool loadFromFlash();

private:
    bool multipleWrites;
    bool written;
};

#endif /*  __ApplicationSettingsH__ */