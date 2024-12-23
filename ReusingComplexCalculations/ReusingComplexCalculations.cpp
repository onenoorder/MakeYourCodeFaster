#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include <chrono>

using namespace std;

struct GasCloudInputData {
    double pressure_ground; // Pressure at ground level (Pa)
    double temperature_ground; // Temperature at ground level (K)
    double molar_mass_air; // Molar mass of air (kg/mol)
    int max_height; // Maximum height (meters)

    double Q; // Source strength (units/s)
    double u; // Wind speed (m/s)
    double sigma_y; // Dispersion parameter in y direction (m)
    double sigma_z; // Dispersion parameter in z direction (m)
    double x; // Distance downwind (m)
    double y; // Crosswind distance (m)
    double z; // Height above ground (m)
    double H; // Effective release height (m)

    int step; // Step size (meters)
    double duration;
    double time_step;
    int num_x;
    int num_y;
    int num_z;
    double grid_size; // meters
};

void run(GasCloudInputData& gasCloudInputData, vector<vector<vector<double>>>& densityArray, std::function<void(GasCloudInputData&, vector<vector<vector<double>>>&)> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    runFunction(gasCloudInputData, densityArray);

    auto finish = std::chrono::high_resolution_clock::now();

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() << "µs\n";
    std::cout << microseconds.count() / 1000 << "ms\n";
    std::cout << microseconds.count() / 1000000 << "s\n";
}

const double R = 8.314; // Ideal gas constant (J/mol K)
const double g = 9.81; // Acceleration due to gravity (m/s^2)

double gaussian_plume_concentration(double Q, double u, double sigma_y, double sigma_z, double x, double y, double z, double H) {
    double pi = 3.14159265359;
    double numerator = Q / (2 * pi * u * sigma_y * sigma_z);
    double exponent_y = -pow(y, 2) / (2 * pow(sigma_y, 2));
    double exponent_z = -pow(z - H, 2) / (2 * pow(sigma_z, 2));
    double concentration = numerator * exp(exponent_y + exponent_z);
    return concentration;
}

double calculate_density(double pressure_ground, double temperature_ground, double molar_mass, double height) {
    // Calculate pressure at height
    double pressure_at_height = pressure_ground * exp(-g * molar_mass * height / (R * temperature_ground));

    // Calculate density
    double density = (pressure_at_height * molar_mass) / (R * temperature_ground);
    return density;
}

double calculate_density(double mass, double volume, double temperature) {
    return mass / volume * R * temperature; // Assuming ideal gas law
}

void simulate_cloud_spreading(double Q, double u, double sigma_y_func(double, char), double sigma_z_func(double, char), double H, double duration, double time_step, double temperature, double molar_mass) {
    for (double t = 0; t <= duration; t += time_step) {
        double x = u * t;
        double sigma_y = sigma_y_func(t, 'A');
        double sigma_z = sigma_z_func(t, 'A');

        // Calculate concentration at the center of the cloud (for simplicity)
        double concentration = gaussian_plume_concentration(Q, u, sigma_y, sigma_z, x, 0, 0, H);

        // Assuming a fixed volume for simplicity
        double volume = 1.0; // Adjust as needed
        double mass = concentration * volume;
        double density = calculate_density(mass, volume, temperature);

        cout << "Time: " << t << " s, Concentration: " << concentration << " units/m^3, Density: " << density << " kg/m^3" << endl;
    }
}

double sigma_y_func(double x, char stability_class) {
    // Simplified example using PGT curves
    double sigma_y;

    switch (stability_class) {
    case 'A':
        sigma_y = 0.22 * pow(x, 0.87);
        break;
    case 'B':
        sigma_y = 0.16 * pow(x, 0.87);
        break;
        // ... other stability classes
    default:
        sigma_y = 0.22 * pow(x, 0.87); // Default to class A
    }

    return sigma_y;
}

double sigma_z_func(double x, char stability_class) {
    // Simplified example using PGT curves
    double sigma_z;

    switch (stability_class) {
    case 'A':
        sigma_z = 0.2 * pow(x, 0.85);
        break;
    case 'B':
        sigma_z = 0.12 * pow(x, 0.85);
        break;
        // ... other stability classes
    default:
        sigma_z = 0.2 * pow(x, 0.85); // Default to class A
    }

    return sigma_z;
}

void create_gas_cloud_density_array(GasCloudInputData& gasCloudInputData, vector<vector<vector<double>>>& densityArray) {
    double x_max = gasCloudInputData.u * gasCloudInputData.duration;
    double y_max = gasCloudInputData.grid_size * gasCloudInputData.num_y;
    double z_max = gasCloudInputData.grid_size * gasCloudInputData.num_z;

    for (int i = 0; i < gasCloudInputData.num_x; ++i) {
        double x = i * gasCloudInputData.grid_size;
        for (int j = 0; j < gasCloudInputData.num_y; ++j) {
            double y = j * gasCloudInputData.grid_size - y_max / 2;
            for (int k = 0; k < gasCloudInputData.num_z; ++k) {
                double z = k * gasCloudInputData.grid_size;
                // Calculate sigma_y and sigma_z based on x
                double sigma_y = sigma_y_func(x, 'A');
                double sigma_z = sigma_z_func(x, 'A');
                double pi = 3.14159265359;
                double numerator = gasCloudInputData.Q / (2 * pi * gasCloudInputData.u * sigma_y * sigma_z);
                double exponent_y = -pow(y, 2) / (2 * pow(sigma_y, 2));
                double exponent_z = -pow(z - gasCloudInputData.H, 2) / (2 * pow(sigma_z, 2));
                double concentration = numerator * exp(exponent_y + exponent_z);
                double density = calculate_density(concentration, 1.0, 20.0);
                densityArray[i][j][k] = density;
            }
        }
    }
}

void create_gas_cloud_density_array_with_reuse(GasCloudInputData& gasCloudInputData, vector<vector<vector<double>>>& densityArray) {
    double x_max = gasCloudInputData.u * gasCloudInputData.duration;
    double y_max = gasCloudInputData.grid_size * gasCloudInputData.num_y;
    double z_max = gasCloudInputData.grid_size * gasCloudInputData.num_z;
    double pi = 3.14159265359;

    for (int i = 0; i < gasCloudInputData.num_x; ++i) {
        double x = i * gasCloudInputData.grid_size;

        // Calculate sigma_y and sigma_z based on x
        double sigma_y = sigma_y_func(x, 'A');
        double sigma_z = sigma_z_func(x, 'A');

        double numerator = gasCloudInputData.Q / (2 * pi * gasCloudInputData.u * sigma_y * sigma_z);
        double exponent_y_pow = (2 * pow(sigma_y, 2));
        double exponent_z_pow = (2 * pow(sigma_z, 2));

        for (int j = 0; j < gasCloudInputData.num_y; ++j) {
            double y = j * gasCloudInputData.grid_size - y_max / 2;

            double exponent_y = -pow(y, 2) / exponent_y_pow;

            for (int k = 0; k < gasCloudInputData.num_z; ++k) {
                double z = k * gasCloudInputData.grid_size;

                double exponent_z = -pow(z - gasCloudInputData.H, 2) / exponent_z_pow;
                double concentration = numerator * exp(exponent_y + exponent_z);
                double density = calculate_density(concentration, 1.0, 20.0);
                densityArray[i][j][k] = density;
            }
        }
    }
}

int main() {
    std::cout << "Reusing Complex Calculations!\n";

    GasCloudInputData gasCloudInputData {
         101325.0,
         288.15,
         0.02897,
         10000,
         10,
         2,
         10,
         5,
         100,
         5,
         2,
         10,
         1,
         3600,
         1,
         1000,
         1000,
         1000,
         10000
    };

    vector<vector<vector<double>>> densityArrayOne(
        gasCloudInputData.num_x, 
        vector<vector<double>>(
            gasCloudInputData.num_y, 
            vector<double>(gasCloudInputData.num_z)
        )
    );

    vector<vector<vector<double>>> densityArrayTwo(
        gasCloudInputData.num_x,
        vector<vector<double>>(
            gasCloudInputData.num_y,
            vector<double>(gasCloudInputData.num_z)
        )
    );

    std::cout << "\n\nCalculate the density of a gas cloud:\n";
    run(gasCloudInputData, densityArrayOne, create_gas_cloud_density_array);

    std::cout << "\n\nCalculate the density of a gas cloud, with reuse of complex calculations:\n";
    run(gasCloudInputData, densityArrayTwo, create_gas_cloud_density_array_with_reuse);
}