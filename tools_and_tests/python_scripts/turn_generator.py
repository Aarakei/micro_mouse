# Generates individual motor pwm values per time step for a turn based off of pre-defined splines and mouse dimensions
# An adaptation from this desmos graph I made: https://www.desmos.com/calculator/8vsw6pltd0

import math

def spline_path(x0,y0,x1,y1,x2,y2,x3,y3,resolution=100):
    """
    Takes in 4 points and generates a parametric list based off of the bezier curve defined by those points
    Default resolution is 100 points (two extra are added to the end to help with future derivatives)
    Returns two lists, the x points of the path, and the y points of the path
    """
    path_x = []
    path_y = []

    # Populate path list
    for i in range(resolution + 2): # i goes from 0 through 101, the extra term lets the final list have 100 elements
        t = i/resolution
        path_x.append(x0*(1-t)**3 + x1*3*t*(1-t)**2 + x2*3*t**2*(1-t) + x3*t**3)
        path_y.append(y0*(1-t)**3 + y1*3*t*(1-t)**2 + y2*3*t**2*(1-t) + y3*t**3)

    return path_x, path_y

def wheel_paths(path_x, path_y, mouse_width):
    """
    Takes in two lists, one for x coordinates and one for y coordinates, of a pre-defined path.
    Calculates the path wheels offset by mouse_width
    returns four lists: the x and y for the left wheel path, and the x and y for the right wheel path
    The length of the returned lists will be one less than the length of the input lists
    """
    left_x = []
    left_y = []
    right_x = []
    right_y = []

    # Populate left and right wheel path lists
    for i in range(len(path_x) - 1): # We're taking a derivative, so the last term would be undefined
        dx = path_x[i+1]-path_x[i]
        dy = path_y[i+1]-path_y[i]
        path_heading = math.atan2(dy,dx)
        left_x.append(path_x[i] + mouse_width*math.cos(path_heading + math.pi/2))
        left_y.append(path_y[i] - mouse_width*math.sin(path_heading - math.pi/2))
        right_x.append(path_x[i] + mouse_width*math.cos(path_heading - math.pi/2))
        right_y.append(path_y[i] - mouse_width*math.sin(path_heading + math.pi/2))

    return left_x, left_y, right_x, right_y

def linear_velocity(path_x, path_y, timestep=10):
    """
    Computes the linear velocity along a given path of x and y points and a given timestep between points in milliseconds
    Returns a list of the velocity at each point
    Total length of the returned list is one less than the length of the input lists
    """
    velocity = []
    dt = timestep / 1000

    for i in range(len(path_x) - 1): # Skip the last term, as it would be undefined
        dx = path_x[i+1] - path_x[i]
        dy = path_y[i+1] - path_y[i]
        vel = math.sqrt(dx**2 + dy**2) / dt
        velocity.append(vel)
    
    return velocity

def convert_velocity_to_pwm(velocity):
    """
    Applies a linear conversion from linear velocities to pwm values
    If a change in pwm does not result in a linear change in velocity, a different conversion should be used
    """
    pwm_list = []

    # Scalar value to multiply velocity by to get pwm
    # Can be found by dividing the max pwm value (often 255) by the mouse's velocity at that pwm
    # 44in*2.54(in/cm)/(31frames/26(frames/s))
    conversion = 255/(44*2.54/(31/26)) 

    # Apply the conversion, then constrain to an integer
    for i in range(len(velocity)):
        pwm = int(velocity[i] * conversion)
        pwm_list.append(pwm)
    
    return pwm_list

def calculate_pwm(x0,y0,x1,y1,x2,y2,x3,y3,mouse_width,resolution=100,timestep=10):
    """
    Takes in points that define a Bezier Curve, the width from the center of rotation to the wheels of the mouse,
    and optionally a resolution (number of points to output), and a timestep(milliseconds per list item)
    by default, they are set to 100 timesteps at 10ms per step
    Returns a list of pwm values for the left and right motors
    """
    left_pwm = []
    right_pwm = []

    path_x, path_y = spline_path(x0,y0,x1,y1,x2,y2,x3,y3,resolution=resolution)

    left_x, left_y, right_x, right_y = wheel_paths(path_x, path_y, mouse_width)
    
    left_pwm = convert_velocity_to_pwm(linear_velocity(left_x, left_y, timestep))
    right_pwm = convert_velocity_to_pwm(linear_velocity(right_x, right_y, timestep))
    
    return left_pwm, right_pwm

def main():
    mouse_width = 3.75 #37.5mm (ish)
    resolution = 225
    timestep = 1
    MAX_PWM = 255
    x0, y0 = 21, 12
    x1, y1 = 25.2426406871, 16.2426406871
    x2, y2 = 25.2426406871, 19.7573593129
    x3, y3 = 21, 24

    left_pwm, right_pwm = calculate_pwm(x0,y0,x1,y1,x2,y2,x3,y3,mouse_width,resolution,timestep)
    max_value = max((max(left_pwm),max(right_pwm)))
    min_value = min((min(left_pwm),min(right_pwm)))
    length = len(left_pwm)
    
    print()
    print(f"left pwm: {left_pwm}")
    print(f"max value: {max(left_pwm)}")
    print()
    print(f"right pwm: {right_pwm}")
    print(f"max value: {max(right_pwm)}")
    print()
    print(f"max: {max_value}")
    print(f"min: {min_value}")
    print(f"length: {length}")
    print(f"time to turn: {resolution*timestep/1000}")
    print()

    suggested_resolution = max_value/MAX_PWM * resolution
    time_to_turn = suggested_resolution*timestep/1000
    print(f"Given current timestep of {timestep}ms:")
    print(f"suggested resolution: {suggested_resolution}")
    print(f"suggested time to turn: {time_to_turn}")
    print(f"suggested timestep for 100 elements: {time_to_turn/100*1000}ms")
    
    print()

if __name__=="__main__":
    main()