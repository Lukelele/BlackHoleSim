#version 410 core

in vec2 v_UV;
out vec4 FragColor;

// Camera uniforms
uniform vec3 u_CameraPos;
uniform vec3 u_CameraForward;
uniform vec3 u_CameraRight;
uniform vec3 u_CameraUp;
uniform vec2 u_Resolution;
uniform float u_FOV;
uniform float u_Time;

// Black Hole General Relativity physics uniforms
uniform float u_Mass;
uniform float u_Spin;
uniform vec3 u_SpinAxis;

// Accretion Disk uniforms
uniform bool u_EnableAccretionDisk;
uniform float u_DiskInnerRadius;
uniform float u_DiskOuterRadius;
uniform float u_DiskBrightness;
uniform float u_DiskTemperature;
uniform float u_DiskScaleHeight;
uniform bool u_EnableDoppler;
uniform bool u_EnableRedshift;

// Ray Tracing quality and visual uniforms
uniform float u_StepFactor;
uniform int u_MaxSteps;
uniform float u_StarfieldBrightness;
uniform float u_Exposure;

#define PI 3.14159265359

// Fast hash for procedural starfield
float hash13(vec3 p) {
    p = fract(p * 0.1031);
    p += dot(p, p.zyx + 31.32);
    return fract((p.x + p.y) * p.z);
}

vec3 hash33(vec3 p) {
    p = fract(p * vec3(0.1031, 0.1030, 0.0973));
    p += dot(p, p.yxz + 33.33);
    return fract((p.xxy + p.yxx) * p.zyx);
}

// Fast blackbody color approximation (T in Kelvin)
vec3 BlackbodyColor(float kelvin) {
    float t = kelvin / 100.0;
    vec3 col;

    if (t <= 66.0) {
        col.r = 1.0;
        col.g = clamp(log(max(1.0, t)) * 0.3900815788 - 0.6318414438, 0.0, 1.0);
    } else {
        col.r = clamp(pow(t - 60.0, -0.1332047592) * 1.2929361860, 0.0, 1.0);
        col.g = clamp(pow(t - 60.0, -0.0755148492) * 1.1298908609, 0.0, 1.0);
    }

    if (t >= 66.0) {
        col.b = 1.0;
    } else if (t <= 19.0) {
        col.b = 0.0;
    } else {
        col.b = clamp(log(max(1.0, t - 10.0)) * 0.5432067891 - 1.1962540891, 0.0, 1.0);
    }

    return col;
}

// Fast procedural deep space celestial skybox (stars, Milky Way galactic plane, nebula)
vec3 GetCelestialSkybox(vec3 dir) {
    vec3 col = vec3(0.001, 0.0015, 0.003);

    // Galactic plane tilted at 30 degrees
    vec3 galAxis = normalize(vec3(0.4, 0.9, 0.1));
    float galLat = dot(dir, galAxis);
    float galBand = exp(-galLat * galLat * 22.0);

    // Galactic center warm core
    vec3 galCenterDir = normalize(vec3(0.8, -0.2, 0.5));
    float galCenter = max(0.0, dot(dir, galCenterDir));
    col += vec3(0.12, 0.06, 0.03) * pow(galCenter, 8.0) * galBand * 2.0;

    // Fast 2-tier starfield
    vec3 p1 = dir * 180.0;
    vec3 id1 = floor(p1);
    vec3 f1 = fract(p1) - 0.5;
    float h1 = hash13(id1);
    if (h1 > 0.985) {
        vec3 starPos = (hash33(id1) - 0.5) * 0.8;
        float d = length(f1 - starPos);
        float brightness = pow(max(0.0, 1.0 - d * 3.5), 10.0) * (h1 - 0.985) * 45.0;
        vec3 starCol = mix(vec3(0.8, 0.9, 1.0), vec3(1.0, 0.7, 0.4), hash13(id1 + 7.7));
        col += starCol * brightness;
    }

    vec3 p2 = dir * 350.0;
    vec3 id2 = floor(p2);
    float h2 = hash13(id2);
    if (h2 > 0.975) {
        col += vec3(0.9, 0.95, 1.0) * ((h2 - 0.975) * 25.0);
    }

    return col * u_StarfieldBrightness;
}

// Optimized General Relativistic Geodesic Acceleration:
// d^2 r / dlambda^2 = - (3 * M * |r x v|^2 / r^5) * r + a_Kerr
vec3 GeodesicAcceleration(vec3 pos, vec3 vel) {
    float r2 = dot(pos, pos);
    if (r2 < 0.0001) return vec3(0.0);

    float rLen = sqrt(r2);
    vec3 L = cross(pos, vel);
    float L2 = dot(L, L);
    float invR5 = 1.0 / (r2 * r2 * rLen);

    vec3 a_schw = (-3.0 * u_Mass * L2 * invR5) * pos;

    if (abs(u_Spin) > 0.001) {
        vec3 S = (u_Spin * u_Mass) * normalize(u_SpinAxis);
        float invR3 = 1.0 / (r2 * rLen);
        vec3 omega_FD = (2.0 * invR3) * S - (3.0 * dot(S, pos) * invR5) * pos;
        return a_schw + 2.0 * cross(vel, omega_FD);
    }

    return a_schw;
}

// Runge-Kutta 4th Order Geodesic Integration Step
void StepRK4(inout vec3 pos, inout vec3 vel, float h) {
    vec3 k1_v = GeodesicAcceleration(pos, vel);
    vec3 k1_r = vel;

    vec3 r2 = pos + 0.5 * h * k1_r;
    vec3 v2 = vel + 0.5 * h * k1_v;
    vec3 k2_v = GeodesicAcceleration(r2, v2);
    vec3 k2_r = v2;

    vec3 r3 = pos + 0.5 * h * k2_r;
    vec3 v3 = vel + 0.5 * h * k2_v;
    vec3 k3_v = GeodesicAcceleration(r3, v3);
    vec3 k3_r = v3;

    vec3 r4 = pos + h * k3_r;
    vec3 v4 = vel + h * k3_v;
    vec3 k4_v = GeodesicAcceleration(r4, v4);
    vec3 k4_r = v4;

    pos += (h * (1.0 / 6.0)) * (k1_r + 2.0 * k2_r + 2.0 * k3_r + k4_r);
    vel += (h * (1.0 / 6.0)) * (k1_v + 2.0 * k2_v + 2.0 * k3_v + k4_v);
}

// Highly optimized Interstellar Gargantua Accretion Disk Model:
// Uses analytical Keplerian spiral shear waves, striated concentric rings, and tuned Doppler beaming
vec3 ComputeGargantuaEmission(vec3 p, vec3 vel, out float outDensity, out float outOpacity) {
    vec3 spinAxis = normalize(u_SpinAxis);
    float hDist = dot(p, spinAxis);
    vec3 diskPlanePos = p - hDist * spinAxis;
    float r = length(diskPlanePos);

    float a = clamp(u_Spin, -0.998, 0.998);
    float rH = u_Mass * (1.0 + sqrt(max(0.0, 1.0 - a * a)));

    // Plunge region between Event Horizon and ISCO
    if (r >= rH * 1.02 && r < u_DiskInnerRadius) {
        float plungeNorm = (r - rH * 1.02) / (u_DiskInnerRadius - rH * 1.02);
        float plungeZ = exp(-abs(hDist) * 35.0);
        float plungeDensity = (1.0 - plungeNorm * plungeNorm) * plungeZ * 0.35;
        outDensity = plungeDensity;
        outOpacity = clamp(plungeDensity * 0.8, 0.0, 1.0);
        return vec3(1.2, 1.3, 1.5) * (4.0 * u_DiskBrightness);
    }

    if (r < u_DiskInnerRadius || r > u_DiskOuterRadius) {
        outDensity = 0.0;
        outOpacity = 0.0;
        return vec3(0.0);
    }

    float H = u_DiskScaleHeight * (pow(r / u_DiskInnerRadius, 0.38) + 0.03);
    float zDist = abs(hDist);
    if (zDist > 3.0 * H) {
        outDensity = 0.0;
        outOpacity = 0.0;
        return vec3(0.0);
    }

    float vertProfile = exp(-0.5 * (zDist * zDist) / (H * H));
    float rNorm = u_DiskInnerRadius / r;
    float radialFlux = pow(rNorm, 2.2) * max(0.0, 1.0 - 0.85 * sqrt(rNorm));

    // Fast analytical Keplerian concentric striations (Interstellar ring structure)
    float ring1 = sin(r * 22.0) * 0.5 + 0.5;
    float ring2 = sin(r * 55.0 + 1.2) * 0.5 + 0.5;
    float ring3 = sin(r * 115.0) * 0.5 + 0.5;
    float striations = 0.52 + 0.28 * ring1 + 0.15 * ring2 + 0.05 * ring3;

    // Keplerian orbital differential shear angle
    float omega = sqrt(u_Mass / (r * r * r));
    float phi = atan(diskPlanePos.z, diskPlanePos.x);
    float shearedAngle = phi - omega * u_Time * 1.4;

    // Fast analytical logarithmic spiral density waves (Lindblad resonance spirals)
    float logR = log(r / u_DiskInnerRadius + 0.05);
    float spiral1 = sin(logR * 12.0 - shearedAngle * 2.0);
    float spiral2 = sin(logR * 24.0 + shearedAngle * 3.0);
    float spiralPattern = 0.65 + 0.25 * spiral1 + 0.10 * spiral2;

    // Fine dust lanes
    float dustLanes = smoothstep(0.2, 0.55, spiralPattern * striations + 0.15);
    float density = vertProfile * radialFlux * striations * spiralPattern * dustLanes;

    if (density < 0.001) {
        outDensity = 0.0;
        outOpacity = 0.0;
        return vec3(0.0);
    }

    // Keplerian orbital velocity v = sqrt(M / r)
    float vOrb = min(0.96, sqrt(u_Mass / r));
    vec3 tangent = normalize(cross(spinAxis, diskPlanePos));
    vec3 beta = tangent * vOrb;

    // Relativistic Doppler factor
    float doppler = 1.0;
    if (u_EnableDoppler) {
        vec3 k = normalize(-vel);
        float betaDotK = dot(beta, k);
        float beta2 = dot(beta, beta);
        float gamma = 1.0 / sqrt(max(0.01, 1.0 - beta2));
        doppler = 1.0 / (gamma * (1.0 - betaDotK));
        doppler = clamp(doppler, 0.22, 5.0);
    }

    // Gravitational redshift
    float gravRedshift = 1.0;
    if (u_EnableRedshift) {
        gravRedshift = sqrt(max(0.0, 1.0 - (2.0 * u_Mass) / r));
    }

    float totalShift = doppler * gravRedshift;

    // Kip Thorne Interstellar beaming curve (p = 2.4 for balanced aesthetic contrast)
    float beaming = u_EnableDoppler ? pow(doppler, 2.4) : 1.0;

    // Blackbody temperature & color
    float localTemp = u_DiskTemperature * pow(radialFlux + 0.05, 0.25);
    float obsTemp = clamp(localTemp * totalShift, 1200.0, 45000.0);
    vec3 bbColor = BlackbodyColor(obsTemp);

    // Warm golden Interstellar palette tint
    vec3 gargantuaTint = mix(vec3(1.0, 0.58, 0.18), vec3(1.0, 0.95, 0.80), pow(radialFlux, 0.45));
    vec3 emitColor = mix(bbColor, bbColor * gargantuaTint, 0.5);

    vec3 emission = emitColor * beaming * density * u_DiskBrightness * 9.0;
    outDensity = density;
    outOpacity = clamp(density * 2.0, 0.0, 1.0);

    return emission;
}

// ACES Filmic Tone Mapping
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    // Generate initial camera ray
    vec2 ndc = (v_UV - 0.5) * 2.0;
    float aspect = u_Resolution.x / u_Resolution.y;
    float tanHalfFov = tan(radians(u_FOV) * 0.5);

    vec3 rayDir = normalize(u_CameraForward +
                            ndc.x * aspect * tanHalfFov * u_CameraRight +
                            ndc.y * tanHalfFov * u_CameraUp);

    vec3 pos = u_CameraPos;
    vec3 vel = rayDir;

    float a = clamp(u_Spin, -0.998, 0.998);
    float rH = u_Mass * (1.0 + sqrt(max(0.0, 1.0 - a * a)));
    float rEscape = 55.0 * u_Mass;
    vec3 spinAxis = normalize(u_SpinAxis);

    vec3 accumColor = vec3(0.0);
    float transmittance = 1.0;
    bool captured = false;

    // Fast, curvature-scaled adaptive geodesic raymarching
    for (int step = 0; step < u_MaxSteps; ++step) {
        float r = length(pos);

        // 1. Event horizon absorption check
        if (r <= rH * 1.008) {
            captured = true;
            break;
        }

        // 2. Escape check
        if (r > rEscape && dot(pos, vel) > 0.0) {
            break;
        }

        // Curvature-scaled step size:
        // Highly refined (0.02 - 0.05) near horizon/photon sphere,
        // and expands gracefully (up to 2.2) in flat space far from the black hole.
        float h = clamp(0.0035 * r * r + 0.015 * r, 0.018, 2.2) * u_StepFactor;

        vec3 nextPos = pos;
        vec3 nextVel = vel;
        StepRK4(nextPos, nextVel, h);

        // 3. Exact Equatorial Plane Crossing Detection (DNGR Method)
        // Only evaluates when the ray crosses the equatorial disk plane
        float hCurr = dot(pos, spinAxis);
        float hNext = dot(nextPos, spinAxis);

        if (u_EnableAccretionDisk && transmittance > 0.01 && (hCurr * hNext <= 0.0)) {
            float t = abs(hCurr) / (abs(hCurr) + abs(hNext) + 1e-6);
            vec3 intersectPos = mix(pos, nextPos, t);
            vec3 intersectVel = mix(vel, nextVel, t);

            float pDensity, pOpacity;
            vec3 planeEmission = ComputeGargantuaEmission(intersectPos, intersectVel, pDensity, pOpacity);
            if (pDensity > 0.001) {
                float planeAlpha = clamp(pOpacity * 1.6, 0.0, 1.0);
                accumColor += transmittance * planeEmission * planeAlpha;
                transmittance *= (1.0 - planeAlpha);
            }
        }

        // 4. Volumetric disk atmosphere: Only evaluated when physically close to the disk volume
        if (u_EnableAccretionDisk && transmittance > 0.01 && abs(hCurr) < 0.45 * u_DiskScaleHeight * r) {
            float vDensity, vOpacity;
            vec3 volEmission = ComputeGargantuaEmission(pos, vel, vDensity, vOpacity);
            if (vDensity > 0.001) {
                float volAlpha = clamp(vOpacity * h * 2.0, 0.0, 1.0);
                accumColor += transmittance * volEmission * volAlpha;
                transmittance *= (1.0 - volAlpha);
            }
        }

        // 5. Spacetime Corona Glow
        if (u_EnableAccretionDisk && transmittance > 0.01 && r > rH && r < u_DiskOuterRadius * 1.3) {
            float coronaIntensity = (1.0 / (r * r + 2.0)) * h * 0.040 * u_DiskBrightness;
            accumColor += transmittance * (vec3(1.0, 0.72, 0.35) * coronaIntensity);
        }

        if (transmittance < 0.005) {
            break;
        }

        pos = nextPos;
        vel = nextVel;
    }

    // 6. Deep space celestial background for escaping rays
    if (!captured && transmittance > 0.005) {
        vec3 bg = GetCelestialSkybox(normalize(vel));
        accumColor += transmittance * bg;
    }

    // 7. Anamorphic lens glow from the accretion disk
    if (u_EnableAccretionDisk) {
        float diskProx = max(0.0, 1.0 - length(ndc * vec2(0.65, 1.4)));
        vec3 anamorphicGlow = vec3(1.0, 0.70, 0.30) * (pow(diskProx, 3.5) * 0.06 * u_DiskBrightness);
        accumColor += anamorphicGlow;
    }

    // Exposure and ACES Filmic Tone Mapping
    vec3 hdrColor = accumColor * u_Exposure;
    vec3 mappedColor = ACESFilm(hdrColor);
    vec3 finalColor = pow(mappedColor, vec3(1.0 / 2.2));

    FragColor = vec4(finalColor, 1.0);
}
