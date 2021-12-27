////////////////////////////////////////////////////////////////////////////////
// Vramsteg - Utility for displaying progress bars in shell scripts.
//
// Copyright 2010 - 2015, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <Progress.h>

////////////////////////////////////////////////////////////////////////////////
Progress::Progress ()
: style ("")
, label ("")
, width (0)
, minimum (0)
, maximum (0)
, current (-1)
, percentage (true)
, remove (true)
, start (0)
, estimate (false)
, elapsed (false)
{
}

////////////////////////////////////////////////////////////////////////////////
Progress::Progress (const std::string& l, int w, long n, long x, bool p /* = true */, bool r /* = true */)
: style ("")
, label (l)
, width (w)
, minimum (n)
, maximum (x)
, percentage (p)
, remove (r)
, start (0)
, estimate (false)
, elapsed (false)
{
  current = -1;
}

////////////////////////////////////////////////////////////////////////////////
Progress::~Progress ()
{
}

////////////////////////////////////////////////////////////////////////////////
void Progress::setStyle (const std::string& value)
{
  style = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::setLabel (const std::string& value)
{
  label = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::setWidth (int value)
{
  width = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::setMin (long value)
{
  minimum = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::setMax (long value)
{
  maximum = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::showPercentage (bool value)
{
  percentage = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::removeAfter (bool value)
{
  remove = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::setStart (time_t value)
{
  start = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::showEstimate (bool value)
{
  estimate = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::showElapsed (bool value)
{
  elapsed = value;
}

////////////////////////////////////////////////////////////////////////////////
void Progress::update (long value)
{
  if (isatty (fileno (stdout)) && current != value)
  {
    // Box the range.
    if (value < minimum) value = minimum;
    if (value > maximum) value = maximum;

    // Current value.
    current = value;

    // Capable of supporting multiple styles.
         if (style == "")     renderStyleDefault ();
    else if (style == "mono") renderStyleMono ();
    else if (style == "text") renderStyleText ();
    else
      throw std::string ("Style '") + style + "' not supported.";
  }
}

////////////////////////////////////////////////////////////////////////////////
void Progress::done ()
{
  if (isatty (fileno (stdout)))
  {
    if (remove)
      std::cout << "\r"
                << std::setfill (' ')
                << std::setw (width)
                << ' ';

    std::cout << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string Progress::formatTime (time_t t)
{
  char buffer [128];

  int days    =  t          / 86400;
  int hours   = (t % 86400) / 3600;
  int minutes = (t %  3600) / 60;
  int seconds =  t % 60;

  if (days)
    snprintf (buffer, 128, "%dd %d:%02d:%02d", days, hours, minutes, seconds);
  else if (hours)
    snprintf (buffer, 128,     "%d:%02d:%02d",       hours, minutes, seconds);
  else
    snprintf (buffer, 128,        "%02d:%02d",              minutes, seconds);

  return std::string (buffer);
}

////////////////////////////////////////////////////////////////////////////////
// Default style looks like this:
//
// label GGGGGGGGRRRRRRRRRRRRRRRRR  34% 0:12 0:35
//
// ^^^^^                                            Label string
//       ^^^^^^^^                                   Completed bar (in green)
//               ^^^^^^^^^^^^^^^^^                  Incomplete bar (in red)
//                                 ^^^^             Percentage complete
//                                      ^^^^        Elapsed time
//                                           ^^^^   Remaining estimate
void Progress::renderStyleDefault ()
{
  // Fraction completed.
  double fraction = (1.0 * (current - minimum)) / (maximum - minimum);

  // Elapsed time.
  time_t now = time (NULL);
  std::string elapsed_time;
  if (elapsed && start != 0)
    elapsed_time = formatTime (now - start);

  // Estimated remaining time.
  std::string estimate_time;
  if (estimate && start != 0)
  {
    if (fraction >= 1e-6)
      estimate_time = formatTime ((time_t) (int) (((now - start) * (1.0 - fraction)) / fraction));
    else
      estimate_time = formatTime (0);
  }

  // Calculate bar width.
  int bar = width
          - (label.length () ? label.length () + 1         : 0)
          - (percentage      ? 5                           : 0)
          - (elapsed         ? elapsed_time.length () + 1  : 0)
          - (estimate        ? estimate_time.length () + 1 : 0);

  if (bar < 1)
    throw std::string ("The specified width is insufficient.");

  int visible = (int) (fraction * bar);

  // Render.
  if (label.length ())
    std::cout << label
              << ' ';

  if (visible > 0)
    std::cout << "\033[42m" // Green
              << std::setfill (' ')
              << std::setw (visible)
              << ' ';

  if (bar - visible > 0)
    std::cout << "\033[41m" // Red
              << std::setfill (' ')
              << std::setw (bar - visible)
              << ' ';

  std::cout << "\033[0m";

  if (percentage)
    std::cout << " "
              << std::setfill (' ')
              << std::setw (3)
              << (int) (fraction * 100)
              << "%";

  if (elapsed && start != 0)
    std::cout << " "
              << elapsed_time;

  if (estimate && start != 0 && fraction > 0.2)
    std::cout << " "
              << estimate_time;

  std::cout << "\r"
            << std::flush;
}

////////////////////////////////////////////////////////////////////////////////
// Mono style looks like this:
//
// label WWWWWWWWBBBBBBBBBBBBBBBBB  34% 0:12 0:35
//
// ^^^^^                                            Label string
//       ^^^^^^^^                                   Completed bar (in white)
//               ^^^^^^^^^^^^^^^^^                  Incomplete bar (in black)
//                                 ^^^^             Percentage complete
//                                      ^^^^        Elapsed time
//                                           ^^^^   Remaining estimate
void Progress::renderStyleMono ()
{
  // Fraction completed.
  double fraction = (1.0 * (current - minimum)) / (maximum - minimum);

  // Elapsed time.
  time_t now = time (NULL);
  std::string elapsed_time;
  if (elapsed && start != 0)
    elapsed_time = formatTime (now - start);

  // Estimated remaining time.
  std::string estimate_time;
  if (estimate && start != 0)
  {
    if (fraction >= 1e-6)
      estimate_time = formatTime ((time_t) (int) (((now - start) * (1.0 - fraction)) / fraction));
    else
      estimate_time = formatTime (0);
  }

  // Calculate bar width.
  int bar = width
          - (label.length () ? label.length () + 1         : 0)
          - (percentage      ? 5                           : 0)
          - (elapsed         ? elapsed_time.length () + 1  : 0)
          - (estimate        ? estimate_time.length () + 1 : 0);

  if (bar < 1)
    throw std::string ("The specified width is insufficient.");

  int visible = (int) (fraction * bar);

  // Render.
  if (label.length ())
    std::cout << label
              << ' ';

  if (visible > 0)
    std::cout << "\033[47m" // White
              << std::setfill (' ')
              << std::setw (visible)
              << ' ';

  if (bar - visible > 0)
    std::cout << "\033[40m" // Black
              << std::setfill (' ')
              << std::setw (bar - visible)
              << ' ';

  std::cout << "\033[0m";

  if (percentage)
    std::cout << " "
              << std::setfill (' ')
              << std::setw (3)
              << (int) (fraction * 100)
              << "%";

  if (elapsed && start != 0)
    std::cout << " "
              << elapsed_time;

  if (estimate && start != 0)
    std::cout << " "
              << estimate_time;

  std::cout << "\r"
            << std::flush;
}

////////////////////////////////////////////////////////////////////////////////
// Text style looks like this:
//
// label [********                ]  34% 0:12 0:35
//
// ^^^^^                                             Label string
//        ^^^^^^^^                                   Completed bar
//               ^^^^^^^^^^^^^^^^^                   Incomplete bar
//                                  ^^^^             Percentage complete
//                                       ^^^^        Elapsed time
//                                            ^^^^   Remaining estimate
void Progress::renderStyleText ()
{
  // Fraction completed.
  double fraction = (1.0 * (current - minimum)) / (maximum - minimum);

  // Elapsed time.
  time_t now = time (NULL);
  std::string elapsed_time;
  if (elapsed && start != 0)
    elapsed_time = formatTime (now - start);

  // Estimated remaining time.
  std::string estimate_time;
  if (estimate && start != 0)
  {
    if (fraction >= 1e-6)
      estimate_time = formatTime ((time_t) (int) (((now - start) * (1.0 - fraction)) / fraction));
    else
      estimate_time = formatTime (0);
  }

  // Calculate bar width.
  int bar = width
          - 2                                                    // The [ and ]
          - (label.length () ? label.length () + 1         : 0)
          - (percentage      ? 5                           : 0)
          - (elapsed         ? elapsed_time.length () + 1  : 0)
          - (estimate        ? estimate_time.length () + 1 : 0);

  if (bar < 1)
    throw std::string ("The specified width is insufficient.");

  int visible = (int) (fraction * bar);

  // Render.
  if (label.length ())
    std::cout << label
              << ' ';

  std::cout << '[';

  if (visible > 0)
    std::cout << std::setfill ('*')
              << std::setw (visible)
              << '*';

  if (bar - visible > 0)
    std::cout << std::setfill (' ')
              << std::setw (bar - visible)
              << ' ';

  std::cout << ']';

  if (percentage)
    std::cout << " "
              << std::setfill (' ')
              << std::setw (3)
              << (int) (fraction * 100)
              << "%";

  if (elapsed && start != 0)
    std::cout << " "
              << elapsed_time;

  if (estimate && start != 0)
    std::cout << " "
              << estimate_time;

  std::cout << "\r"
            << std::flush;
}

////////////////////////////////////////////////////////////////////////////////
