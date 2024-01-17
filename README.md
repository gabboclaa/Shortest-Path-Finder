# Shortest-Path-Finder

**Project Summary: Algorithm and Data Structures Final Exam 2022-2023**

Consider an expressway represented as a sequence of service stations, each located at a specific distance from the beginning of the highway in kilometers, expressed as a non-negative integer. No two service stations share the same distance, uniquely identifying each station.

Each service station is equipped with an electric vehicle rental fleet. Each vehicle has a range defined by the battery charge, expressed in kilometers as a positive integer. A station's vehicle fleet comprises a maximum of 512 vehicles. Renting a car from station s allows reaching all stations within the vehicle's range from s.

A journey is identified by a sequence of service stations where the driver makes stops, starting and ending at different stations, with possible intermediate stops. The driver cannot backtrack during the journey, obtaining a new rental car at each station stop. Thus, given two consecutive stops s and t, t must be farther from the starting point than s, and it must be reachable using one of the vehicles available at s.

The project's goal is to plan the route with the minimum number of stops between a given pair of stations. In cases where multiple routes have the same minimum number of stops, the preference is given to the route favoring shorter distances from the beginning of the expressway. Formally, given an equal-rank set of n paths P = {p1, p2, ..., pn}, where each path is a tuple of m elements pi = ⟨pi,1, pi,2, ..., pi,m⟩ representing the distance from the start of the expressway for each stop in the order of traversal, the chosen path is pi such that no other path pj exists with the same k final stops preceded by a stop at a shorter distance, i.e., ∄j, k: ⟨pi,m−k+1, ..., pi,m⟩ = ⟨pj,m−k+1, ..., pj,m⟩ ∧ pj,m−k < pi,m−k.
