#pragma once


namespace planet::sdl {


    /// ## Conventions taken from the machine the game is run on
    /**
     * Not part of the engine's interface: a game gets this by starting the
     * engine, so it sits beside the `init` that calls it, where the tests over
     * it can still reach it.
     */

    /// ### Write dates and times the way this machine writes them
    void use_system_time_conventions();
    /**
     * Called by `init`, after which a date shows as `04/08/26` where the day
     * leads and `08/04/2026 10:42:34 AM` where the month does.
     *
     * A locale named in the environment is the one taken up, including when it
     * names `C`. SDL is asked only when the environment says nothing — which is
     * how a macOS application launched from the Finder is started, leaving the
     * C library nothing to go on where SDL still knows to ask the platform.
     *
     * Only `LC_TIME` is handed over: dates are what is wanted, where the
     * numeric conventions would change how the C library reads and writes every
     * number in the process.
     *
     * A machine that says nothing anywhere keeps the `C` conventions, which are
     * still a readable date and time, so this logs rather than refusing to
     * start.
     */


}
