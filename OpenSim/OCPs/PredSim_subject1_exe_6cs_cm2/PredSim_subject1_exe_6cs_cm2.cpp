#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/SimbodyEngine/PlanarJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/PinJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/Joint.h>
#include <OpenSim/Simulation/SimbodyEngine/SpatialTransform.h>
#include <OpenSim/Simulation/SimbodyEngine/CustomJoint.h>
#include <OpenSim/Common/LinearFunction.h>
#include <OpenSim/Common/Constant.h>
#include <OpenSim/Common/SimmSpline.h>
#include <OpenSim/Simulation/Model/ConditionalPathPoint.h>
#include <OpenSim/Simulation/Model/MovingPathPoint.h>
#include <OpenSim/Simulation/Model/HuntCrossleyForce_smooth.h>
#include <recorder.hpp>

#include <iostream>
#include <iterator>
#include <random>
#include <cassert>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace SimTK;
using namespace OpenSim;

// Declare inputs/outputs of function F
/// number of vectors in inputs/outputs of function F
constexpr int n_in = 2;
constexpr int n_out = 1;
/// number of elements in input/output vectors of function F
constexpr int ndof = 29;        // degrees of freedom (excluding locked)
constexpr int ndofr = 31;       // degrees of freedom (including locked)
constexpr int NX = ndof*2;      // states
constexpr int NU = ndof;        // controls
constexpr int NR = ndof+6+6;    // residual torques + GRFs + Origins calcaneus.

/// Value
template<typename T>
T value(const Recorder& e) { return e; }

template<>
double value(const Recorder& e) { return e.getValue(); }

/* Function F, using templated type T
F(x,u) -> (tau)
*/

template<typename T>
int F_generic(const T** arg, T** res) {

    // OpenSim model
    OpenSim::Model* model;
    OpenSim::Body* pelvis;
    OpenSim::Body* femur_r;
    OpenSim::Body* femur_l;
    OpenSim::Body* tibia_r;
    OpenSim::Body* tibia_l;
    OpenSim::Body* talus_r;
    OpenSim::Body* talus_l;
    OpenSim::Body* calcn_r;
    OpenSim::Body* calcn_l;
    OpenSim::Body* toes_r;
    OpenSim::Body* toes_l;
    OpenSim::Body* torso;
    OpenSim::Body* humerus_r;
    OpenSim::Body* humerus_l;
    OpenSim::Body* ulna_r;
    OpenSim::Body* ulna_l;
    OpenSim::Body* radius_r;
    OpenSim::Body* radius_l;
    OpenSim::Body* hand_r;
    OpenSim::Body* hand_l;
    OpenSim::CustomJoint* ground_pelvis;
    OpenSim::CustomJoint* hip_r;
    OpenSim::CustomJoint* hip_l;
    OpenSim::CustomJoint* knee_r;
    OpenSim::CustomJoint* knee_l;
    OpenSim::CustomJoint* ankle_r;
    OpenSim::CustomJoint* ankle_l;
    OpenSim::CustomJoint* subtalar_r;
    OpenSim::CustomJoint* subtalar_l;
    OpenSim::WeldJoint* mtp_r;
    OpenSim::WeldJoint* mtp_l;
    OpenSim::CustomJoint* back;
    OpenSim::CustomJoint* shoulder_r;
    OpenSim::CustomJoint* shoulder_l;
    OpenSim::CustomJoint* elbow_r;
    OpenSim::CustomJoint* elbow_l;
    OpenSim::CustomJoint* radioulnar_r;
    OpenSim::CustomJoint* radioulnar_l;
    OpenSim::WeldJoint* radius_hand_r;
    OpenSim::WeldJoint* radius_hand_l;
    /// contact elements
    OpenSim::HuntCrossleyForce_smooth* HC_1_r;
    OpenSim::HuntCrossleyForce_smooth* HC_2_r;
    OpenSim::HuntCrossleyForce_smooth* HC_3_r;
    OpenSim::HuntCrossleyForce_smooth* HC_4_r;
    OpenSim::HuntCrossleyForce_smooth* HC_5_r;
    OpenSim::HuntCrossleyForce_smooth* HC_6_r;
    const Vec3 locSphere_1_r =
        Vec3(-0.015049446786385, -0.021859, 0.010117932234578);
    const Vec3 locSphere_2_r =
        Vec3(0.123406401693187, -0.021859, -0.003836159109877);
    const Vec3 locSphere_3_r =
        Vec3(0.108012428116283, -0.021859, 0.026646807301930);
    const Vec3 locSphere_4_r =
        Vec3(0.037581878945575, -0.0214476, -0.010596005235719);
    const Vec3 locSphere_5_r =
        Vec3(0.041236827702690, -0.021859, 0.001388254238589);
    const Vec3 locSphere_6_r =
        Vec3(0.029080671438514, -0.0214476, 0.036859228220522);

    OpenSim::HuntCrossleyForce_smooth* HC_1_l;
    OpenSim::HuntCrossleyForce_smooth* HC_2_l;
    OpenSim::HuntCrossleyForce_smooth* HC_3_l;
    OpenSim::HuntCrossleyForce_smooth* HC_4_l;
    OpenSim::HuntCrossleyForce_smooth* HC_5_l;
    OpenSim::HuntCrossleyForce_smooth* HC_6_l;
    const Vec3 locSphere_1_l =
        Vec3(-0.015049446786385, -0.021859, -0.010117932234578);
    const Vec3 locSphere_2_l =
        Vec3(0.123406401693187, -0.021859, 0.003836159109877);
    const Vec3 locSphere_3_l =
        Vec3(0.108012428116283, -0.021859, -0.026646807301930);
    const Vec3 locSphere_4_l =
        Vec3(0.037581878945575, -0.0214476, 0.010596005235719);
    const Vec3 locSphere_5_l =
        Vec3(0.041236827702690, -0.021859, -0.001388254238589);
    const Vec3 locSphere_6_l =
        Vec3(0.029080671438514, -0.0214476, -0.036859228220522);

    /// Model
    model = new OpenSim::Model();
    /// Bodies - Definition
    pelvis = new OpenSim::Body("pelvis", 8.84259166189724, Vec3(-0.0682778, 0, 0), Inertia(0.0741799006400181, 0.0741799006400181, 0.0405455944309864, 0, 0, 0));
    femur_l = new OpenSim::Body("femur_l", 6.98382288222561, Vec3(0, -0.170467, 0), Inertia(0.101089610270247, 0.0264992182261812, 0.106600843690507, 0, 0, 0));
    femur_r = new OpenSim::Body("femur_r", 6.98382288222561, Vec3(0, -0.170467, 0), Inertia(0.101089610270247, 0.0264992182261812, 0.106600843690507, 0, 0, 0));
    tibia_l = new OpenSim::Body("tibia_l", 2.78372323906632, Vec3(0, -0.180489, 0), Inertia(0.0353661477848549, 0.00357871733537223, 0.0358573442818668, 0, 0, 0));
    tibia_r = new OpenSim::Body("tibia_r", 2.78372323906632, Vec3(0, -0.180489, 0), Inertia(0.0353661477848549, 0.00357871733537223, 0.0358573442818668, 0, 0, 0));
    talus_l = new OpenSim::Body("talus_l", 0.0750835667988218, Vec3(0, 0, 0), Inertia(0.00062714132461258, 0.00062714132461258, 0.00062714132461258, 0, 0, 0));
    talus_r = new OpenSim::Body("talus_r", 0.0750835667988218, Vec3(0, 0, 0), Inertia(0.00062714132461258, 0.00062714132461258, 0.00062714132461258, 0, 0, 0));
    calcn_l = new OpenSim::Body("calcn_l", 0.938544584985273, Vec3(0.0913924, 0.0274177, 0), Inertia(0.000877997854457612, 0.00244585116598906, 0.00257127943091158, 0, 0, 0));
    calcn_r = new OpenSim::Body("calcn_r", 0.938544584985273, Vec3(0.0913924, 0.0274177, 0), Inertia(0.000877997854457612, 0.00244585116598906, 0.00257127943091158, 0, 0, 0));
    toes_l = new OpenSim::Body("toes_l", 0.162631005686248, Vec3(0.0316218, 0.00548355, 0.0159937), Inertia(6.2714132461258e-005, 0.000125428264922516, 6.2714132461258e-005, 0, 0, 0));
    toes_r = new OpenSim::Body("toes_r", 0.162631005686248, Vec3(0.0316218, 0.00548355, -0.0159937), Inertia(6.2714132461258e-005, 0.000125428264922516, 6.2714132461258e-005, 0, 0, 0));
    torso = new OpenSim::Body("torso", 25.7060604306454, Vec3(-0.0267603, 0.306505, 0), Inertia(0.981166155448334, 0.451354452950527, 0.981166155448334, 0, 0, 0));
    humerus_l = new OpenSim::Body("humerus_l", 1.52611854532613, Vec3(0, -0.169033, 0), Inertia(0.00947044247669374, 0.00326700932918591, 0.0106302664632502, 0, 0, 0));
    humerus_r = new OpenSim::Body("humerus_r", 1.52611854532613, Vec3(0, -0.169033, 0), Inertia(0.00947044247669374, 0.00326700932918591, 0.0106302664632502, 0, 0, 0));
    ulna_l = new OpenSim::Body("ulna_l", 0.456132668302843, Vec3(0, -0.118275, 0), Inertia(0.00214171997483337, 0.000446854471454093, 0.00232320941226861, 0, 0, 0));
    ulna_r = new OpenSim::Body("ulna_r", 0.456132668302843, Vec3(0, -0.118275, 0), Inertia(0.00214171997483337, 0.000446854471454093, 0.00232320941226861, 0, 0, 0));
    radius_l = new OpenSim::Body("radius_l", 0.456132668302843, Vec3(0, -0.118275, 0), Inertia(0.00214171997483337, 0.000446854471454093, 0.00232320941226861, 0, 0, 0));
    radius_r = new OpenSim::Body("radius_r", 0.456132668302843, Vec3(0, -0.118275, 0), Inertia(0.00214171997483337, 0.000446854471454093, 0.00232320941226861, 0, 0, 0));
    hand_l = new OpenSim::Body("hand_l", 0.34350731810461, Vec3(0, -0.0668239, 0), Inertia(0.000644974415108496, 0.000395516821821017, 0.000968907753638324, 0, 0, 0));
    hand_r = new OpenSim::Body("hand_r", 0.34350731810461, Vec3(0, -0.0668239, 0), Inertia(0.000644974415108496, 0.000395516821821017, 0.000968907753638324, 0, 0, 0));
    /// Joints - Transforms
    // Ground-Pelvis
    SpatialTransform st_ground_pelvis;
    st_ground_pelvis[0].setCoordinateNames(OpenSim::Array<std::string>("pelvis_tilt", 1, 1));
    st_ground_pelvis[0].setFunction(new LinearFunction());
    st_ground_pelvis[0].setAxis(Vec3(0, 0, 1));
    st_ground_pelvis[1].setCoordinateNames(OpenSim::Array<std::string>("pelvis_list", 1, 1));
    st_ground_pelvis[1].setFunction(new LinearFunction());
    st_ground_pelvis[1].setAxis(Vec3(1, 0, 0));
    st_ground_pelvis[2].setCoordinateNames(OpenSim::Array<std::string>("pelvis_rotation", 1, 1));
    st_ground_pelvis[2].setFunction(new LinearFunction());
    st_ground_pelvis[2].setAxis(Vec3(0, 1, 0));
    st_ground_pelvis[3].setCoordinateNames(OpenSim::Array<std::string>("pelvis_tx", 1, 1));
    st_ground_pelvis[3].setFunction(new LinearFunction());
    st_ground_pelvis[3].setAxis(Vec3(1, 0, 0));
    st_ground_pelvis[4].setCoordinateNames(OpenSim::Array<std::string>("pelvis_ty", 1, 1));
    st_ground_pelvis[4].setFunction(new LinearFunction());
    st_ground_pelvis[4].setAxis(Vec3(0, 1, 0));
    st_ground_pelvis[5].setCoordinateNames(OpenSim::Array<std::string>("pelvis_tz", 1, 1));
    st_ground_pelvis[5].setFunction(new LinearFunction());
    st_ground_pelvis[5].setAxis(Vec3(0, 0, 1));
    // Hip_l
    SpatialTransform st_hip_l;
    st_hip_l[0].setCoordinateNames(OpenSim::Array<std::string>("hip_flexion_l", 1, 1));
    st_hip_l[0].setFunction(new LinearFunction());
    st_hip_l[0].setAxis(Vec3(0, 0, 1));
    st_hip_l[1].setCoordinateNames(OpenSim::Array<std::string>("hip_adduction_l", 1, 1));
    st_hip_l[1].setFunction(new LinearFunction());
    st_hip_l[1].setAxis(Vec3(-1, 0, 0));
    st_hip_l[2].setCoordinateNames(OpenSim::Array<std::string>("hip_rotation_l", 1, 1));
    st_hip_l[2].setFunction(new LinearFunction());
    st_hip_l[2].setAxis(Vec3(0, -1, 0));
    // Hip_r
    SpatialTransform st_hip_r;
    st_hip_r[0].setCoordinateNames(OpenSim::Array<std::string>("hip_flexion_r", 1, 1));
    st_hip_r[0].setFunction(new LinearFunction());
    st_hip_r[0].setAxis(Vec3(0, 0, 1));
    st_hip_r[1].setCoordinateNames(OpenSim::Array<std::string>("hip_adduction_r", 1, 1));
    st_hip_r[1].setFunction(new LinearFunction());
    st_hip_r[1].setAxis(Vec3(1, 0, 0));
    st_hip_r[2].setCoordinateNames(OpenSim::Array<std::string>("hip_rotation_r", 1, 1));
    st_hip_r[2].setFunction(new LinearFunction());
    st_hip_r[2].setAxis(Vec3(0, 1, 0));
    // Knee_l
    SpatialTransform st_knee_l;
    st_knee_l[2].setCoordinateNames(OpenSim::Array<std::string>("knee_angle_l", 1, 1));
    st_knee_l[2].setFunction(new LinearFunction());
    st_knee_l[2].setAxis(Vec3(0, 0, 1));
    // Knee_r
    SpatialTransform st_knee_r;
    st_knee_r[2].setCoordinateNames(OpenSim::Array<std::string>("knee_angle_r", 1, 1));
    st_knee_r[2].setFunction(new LinearFunction());
    st_knee_r[2].setAxis(Vec3(0, 0, 1));
    // Ankle_l
    SpatialTransform st_ankle_l;
    st_ankle_l[0].setCoordinateNames(OpenSim::Array<std::string>("ankle_angle_l", 1, 1));
    st_ankle_l[0].setFunction(new LinearFunction());
    st_ankle_l[0].setAxis(Vec3(0.10501355, 0.17402245, 0.97912632));
    // Ankle_r
    SpatialTransform st_ankle_r;
    st_ankle_r[0].setCoordinateNames(OpenSim::Array<std::string>("ankle_angle_r", 1, 1));
    st_ankle_r[0].setFunction(new LinearFunction());
    st_ankle_r[0].setAxis(Vec3(-0.10501355, -0.17402245, 0.97912632));
    // Subtalar_l
    SpatialTransform st_subtalar_l;
    st_subtalar_l[0].setCoordinateNames(OpenSim::Array<std::string>("subtalar_angle_l", 1, 1));
    st_subtalar_l[0].setFunction(new LinearFunction());
    st_subtalar_l[0].setAxis(Vec3(-0.78717961, -0.60474746, -0.12094949));
    // Subtalar_r
    SpatialTransform st_subtalar_r;
    st_subtalar_r[0].setCoordinateNames(OpenSim::Array<std::string>("subtalar_angle_r", 1, 1));
    st_subtalar_r[0].setFunction(new LinearFunction());
    st_subtalar_r[0].setAxis(Vec3(0.78717961, 0.60474746, -0.12094949));
    // Back
    SpatialTransform st_back;
    st_back[0].setCoordinateNames(OpenSim::Array<std::string>("lumbar_extension", 1, 1));
    st_back[0].setFunction(new LinearFunction());
    st_back[0].setAxis(Vec3(0, 0, 1));
    st_back[1].setCoordinateNames(OpenSim::Array<std::string>("lumbar_bending", 1, 1));
    st_back[1].setFunction(new LinearFunction());
    st_back[1].setAxis(Vec3(1, 0, 0));
    st_back[2].setCoordinateNames(OpenSim::Array<std::string>("lumbar_rotation", 1, 1));
    st_back[2].setFunction(new LinearFunction());
    st_back[2].setAxis(Vec3(0, 1, 0));
    // shoulder_l
    SpatialTransform st_sho_l;
    st_sho_l[0].setCoordinateNames(OpenSim::Array<std::string>("arm_flex_l", 1, 1));
    st_sho_l[0].setFunction(new LinearFunction());
    st_sho_l[0].setAxis(Vec3(0, 0, 1));
    st_sho_l[1].setCoordinateNames(OpenSim::Array<std::string>("arm_add_l", 1, 1));
    st_sho_l[1].setFunction(new LinearFunction());
    st_sho_l[1].setAxis(Vec3(-1, 0, 0));
    st_sho_l[2].setCoordinateNames(OpenSim::Array<std::string>("arm_rot_l", 1, 1));
    st_sho_l[2].setFunction(new LinearFunction());
    st_sho_l[2].setAxis(Vec3(0, -1, 0));
    // shoulder_r
    SpatialTransform st_sho_r;
    st_sho_r[0].setCoordinateNames(OpenSim::Array<std::string>("arm_flex_r", 1, 1));
    st_sho_r[0].setFunction(new LinearFunction());
    st_sho_r[0].setAxis(Vec3(0, 0, 1));
    st_sho_r[1].setCoordinateNames(OpenSim::Array<std::string>("arm_add_r", 1, 1));
    st_sho_r[1].setFunction(new LinearFunction());
    st_sho_r[1].setAxis(Vec3(1, 0, 0));
    st_sho_r[2].setCoordinateNames(OpenSim::Array<std::string>("arm_rot_r", 1, 1));
    st_sho_r[2].setFunction(new LinearFunction());
    st_sho_r[2].setAxis(Vec3(0, 1, 0));
    // elbow_l
    SpatialTransform st_elb_l;
    st_elb_l[0].setCoordinateNames(OpenSim::Array<std::string>("elbow_flex_l", 1, 1));
    st_elb_l[0].setFunction(new LinearFunction());
    st_elb_l[0].setAxis(Vec3(-0.22604696, -0.022269, 0.97386183));
    // elbow_r
    SpatialTransform st_elb_r;
    st_elb_r[0].setCoordinateNames(OpenSim::Array<std::string>("elbow_flex_r", 1, 1));
    st_elb_r[0].setFunction(new LinearFunction());
    st_elb_r[0].setAxis(Vec3(0.22604696, 0.022269, 0.97386183));
    // radioulnar_l
    SpatialTransform st_radioulnar_l;
    st_radioulnar_l[0].setCoordinateNames(OpenSim::Array<std::string>("pro_sup_l", 1, 1));
    st_radioulnar_l[0].setFunction(new LinearFunction());
    st_radioulnar_l[0].setAxis(Vec3(-0.05639803, -0.99840646, 0.001952));
    // radioulnar_r
    SpatialTransform st_radioulnar_r;
    st_radioulnar_r[0].setCoordinateNames(OpenSim::Array<std::string>("pro_sup_r", 1, 1));
    st_radioulnar_r[0].setFunction(new LinearFunction());
    st_radioulnar_r[0].setAxis(Vec3(0.05639803, 0.99840646, 0.001952));
    /// Joints - Definition
    ground_pelvis = new CustomJoint("ground_pelvis", model->getGround(), Vec3(0), Vec3(0), *pelvis, Vec3(0), Vec3(0), st_ground_pelvis);
    hip_l = new CustomJoint("hip_l", *pelvis, Vec3(-0.0682778001711179, -0.0638353973311301, -0.0823306940058688), Vec3(0), *femur_l, Vec3(0), Vec3(0), st_hip_l);
    hip_r = new CustomJoint("hip_r", *pelvis, Vec3(-0.0682778001711179, -0.0638353973311301, 0.0823306940058688), Vec3(0), *femur_r, Vec3(0), Vec3(0), st_hip_r);
    knee_l = new CustomJoint("knee_l", *femur_l, Vec3(-0.00451221232146798, -0.396907245921447, 0), Vec3(0), *tibia_l, Vec3(0), Vec3(0), st_knee_l);
    knee_r = new CustomJoint("knee_r", *femur_r, Vec3(-0.00451221232146798, -0.396907245921447, 0), Vec3(0), *tibia_r, Vec3(0), Vec3(0), st_knee_r);
    ankle_l = new CustomJoint("ankle_l", *tibia_l, Vec3(0, -0.415694825374905, 0), Vec3(0), *talus_l, Vec3(0), Vec3(0), st_ankle_l);
    ankle_r = new CustomJoint("ankle_r", *tibia_r, Vec3(0, -0.415694825374905, 0), Vec3(0), *talus_r, Vec3(0), Vec3(0), st_ankle_r);
    subtalar_l = new CustomJoint("subtalar_l", *talus_l, Vec3(-0.0445720919117321, -0.0383391276542374, -0.00723828107321956), Vec3(0), *calcn_l, Vec3(0), Vec3(0),st_subtalar_l);
    subtalar_r = new CustomJoint("subtalar_r", *talus_r, Vec3(-0.0445720919117321, -0.0383391276542374, 0.00723828107321956), Vec3(0), *calcn_r, Vec3(0), Vec3(0),st_subtalar_r);
    mtp_l = new WeldJoint("mtp_l", *calcn_l, Vec3(0.163409678774199, -0.00182784875586352, -0.000987038328166303), Vec3(0), *toes_l, Vec3(0), Vec3(0));
    mtp_r = new WeldJoint("mtp_r", *calcn_r, Vec3(0.163409678774199, -0.00182784875586352, 0.000987038328166303), Vec3(0), *toes_r, Vec3(0), Vec3(0));
    back = new CustomJoint("back", *pelvis, Vec3(-0.0972499926058214, 0.0787077894476112, 0), Vec3(0), *torso, Vec3(0), Vec3(0), st_back);
    shoulder_l = new CustomJoint("shoulder_l", *torso, Vec3(0.0028142880546385, 0.35583331053375, -0.151641511660395), Vec3(0), *humerus_l, Vec3(0), Vec3(0), st_sho_l);
    shoulder_r = new CustomJoint("shoulder_r", *torso, Vec3(0.0028142880546385, 0.35583331053375, 0.151641511660395), Vec3(0), *humerus_r, Vec3(0), Vec3(0), st_sho_r);
    elbow_l = new CustomJoint("elbow_l", *humerus_l, Vec3(0.0135060695814636, -0.294158784030305, 0.00985930748890318), Vec3(0), *ulna_l, Vec3(0), Vec3(0), st_elb_l);
    elbow_r = new CustomJoint("elbow_r", *humerus_r, Vec3(0.0135060695814636, -0.294158784030305, -0.00985930748890318), Vec3(0), *ulna_r, Vec3(0), Vec3(0), st_elb_r);
    radioulnar_l = new CustomJoint("radioulnar_l", *ulna_l, Vec3(-0.00660142656498441, -0.0127641973139218, -0.0255961065994483), Vec3(0), *radius_l, Vec3(0), Vec3(0),st_radioulnar_l);
    radioulnar_r = new CustomJoint("radioulnar_r", *ulna_r, Vec3(-0.00660142656498441, -0.0127641973139218, 0.0255961065994483), Vec3(0), *radius_r, Vec3(0), Vec3(0),st_radioulnar_r);
    radius_hand_l = new WeldJoint("radius_hand_l", *radius_l, Vec3(-0.00863278571312143, -0.231438537611489, -0.0133559410657705), Vec3(0), *hand_l, Vec3(0), Vec3(0));
    radius_hand_r = new WeldJoint("radius_hand_r", *radius_r, Vec3(-0.00863278571312143, -0.231438537611489, 0.0133559410657705), Vec3(0), *hand_r, Vec3(0), Vec3(0));
    /// bodies and joints
    model->addBody(pelvis);		    model->addJoint(ground_pelvis);
    model->addBody(femur_l);		model->addJoint(hip_l);
    model->addBody(femur_r);		model->addJoint(hip_r);
    model->addBody(tibia_l);		model->addJoint(knee_l);
    model->addBody(tibia_r);		model->addJoint(knee_r);
    model->addBody(talus_l);		model->addJoint(ankle_l);
    model->addBody(talus_r);		model->addJoint(ankle_r);
    model->addBody(calcn_l);		model->addJoint(subtalar_l);
    model->addBody(calcn_r);		model->addJoint(subtalar_r);
    model->addBody(toes_l);		    model->addJoint(mtp_l);
    model->addBody(toes_r);		    model->addJoint(mtp_r);
    model->addBody(torso);          model->addJoint(back);
    model->addBody(humerus_l);      model->addJoint(shoulder_l);
    model->addBody(humerus_r);      model->addJoint(shoulder_r);
    model->addBody(ulna_l);         model->addJoint(elbow_l);
    model->addBody(ulna_r);         model->addJoint(elbow_r);
    model->addBody(radius_l);       model->addJoint(radioulnar_l);
    model->addBody(radius_r);       model->addJoint(radioulnar_r);
    model->addBody(hand_l);         model->addJoint(radius_hand_l);
    model->addBody(hand_r);         model->addJoint(radius_hand_r);
    /// Contact elements
    osim_double_adouble radiusSphere = 0.032;
    osim_double_adouble stiffness_s1 = 1.499957161869131e+06;
    osim_double_adouble stiffness_s2 = 5.001094706457405e+05;
    osim_double_adouble stiffness_s3 = 5.001038051955853e+05;
    osim_double_adouble stiffness_s4 = 9.190016350055737e+05;
    osim_double_adouble stiffness_s5 = 1.499913937079755e+06;
    osim_double_adouble stiffness_s6 = 5.000417020349433e+05;
    osim_double_adouble dissipation = 2.0;
    osim_double_adouble staticFriction = 0.8;
    osim_double_adouble dynamicFriction = 0.8;
    osim_double_adouble viscousFriction = 0.5;
    osim_double_adouble transitionVelocity = 0.2;
    Vec3 normal = Vec3(0, 1, 0);
    osim_double_adouble offset = 0;
    /// Left - Definition
    HC_1_l = new HuntCrossleyForce_smooth("sphere_1_l", "calcn_l", locSphere_1_l, radiusSphere,
        stiffness_s1, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_2_l = new HuntCrossleyForce_smooth("sphere_2_l", "calcn_l", locSphere_2_l, radiusSphere,
        stiffness_s2, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_3_l = new HuntCrossleyForce_smooth("sphere_3_l", "calcn_l", locSphere_3_l, radiusSphere,
        stiffness_s3, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_4_l = new HuntCrossleyForce_smooth("sphere_4_l", "toes_l", locSphere_4_l, radiusSphere,
        stiffness_s4, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_5_l = new HuntCrossleyForce_smooth("sphere_5_l", "calcn_l", locSphere_5_l, radiusSphere,
        stiffness_s5, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_6_l = new HuntCrossleyForce_smooth("sphere_6_l", "toes_l", locSphere_6_l, radiusSphere,
        stiffness_s6, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    model->addComponent(HC_1_l);
    HC_1_l->connectSocket_body_sphere(*calcn_l);
    model->addComponent(HC_2_l);
    HC_2_l->connectSocket_body_sphere(*calcn_l);
    model->addComponent(HC_3_l);
    HC_3_l->connectSocket_body_sphere(*calcn_l);
    model->addComponent(HC_4_l);
    HC_4_l->connectSocket_body_sphere(*toes_l);
    model->addComponent(HC_5_l);
    HC_5_l->connectSocket_body_sphere(*calcn_l);
    model->addComponent(HC_6_l);
    HC_6_l->connectSocket_body_sphere(*toes_l);
    /// Right - Definition
    HC_1_r = new HuntCrossleyForce_smooth("sphere_1_r", "calcn_r", locSphere_1_r, radiusSphere,
        stiffness_s1, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_2_r = new HuntCrossleyForce_smooth("sphere_2_r", "calcn_r", locSphere_2_r, radiusSphere,
        stiffness_s2, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_3_r = new HuntCrossleyForce_smooth("sphere_3_r", "calcn_r", locSphere_3_r, radiusSphere,
        stiffness_s3, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_4_r = new HuntCrossleyForce_smooth("sphere_4_r", "toes_r", locSphere_4_r, radiusSphere,
        stiffness_s4, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_5_r = new HuntCrossleyForce_smooth("sphere_5_r", "calcn_r", locSphere_5_r, radiusSphere,
        stiffness_s5, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    HC_6_r = new HuntCrossleyForce_smooth("sphere_6_r", "toes_r", locSphere_6_r, radiusSphere,
        stiffness_s6, dissipation, staticFriction, dynamicFriction, viscousFriction, transitionVelocity, normal, offset);
    model->addComponent(HC_1_r);
    HC_1_r->connectSocket_body_sphere(*calcn_r);
    model->addComponent(HC_2_r);
    HC_2_r->connectSocket_body_sphere(*calcn_r);
    model->addComponent(HC_3_r);
    HC_3_r->connectSocket_body_sphere(*calcn_r);
    model->addComponent(HC_4_r);
    HC_4_r->connectSocket_body_sphere(*toes_r);
    model->addComponent(HC_5_r);
    HC_5_r->connectSocket_body_sphere(*calcn_r);
    model->addComponent(HC_6_r);
    HC_6_r->connectSocket_body_sphere(*toes_r);
    /// Initialize  system and state.
    /// states
    SimTK::State* state;
    state = new State(model->initSystem());

    // Read inputs
    std::vector<T> x(arg[0], arg[0] + NX);
    std::vector<T> u(arg[1], arg[1] + NU);

    // States and controls
    T ua[NU+2]; /// joint accelerations (Qdotdots) - controls
    Vector QsUs(NX+4); /// joint positions (Qs) and velocities (Us) - states

    // Assign inputs to model variables
    // states
    for (int i = 0; i < NX; ++i) QsUs[i] = x[i];
    // pro_sup dofs are locked so we hard code the Qs and Qdots
    QsUs[NX] = 1.51;
    QsUs[NX+1] = 0;
    QsUs[NX+2] = 1.51;
    QsUs[NX+3] = 0;
    for (int i = 0; i < 12; ++i) ua[i] = u[i];      // controls
    ua[12] = u[18]; // 12 Simbody () is 18 OpenSim
    ua[13] = u[19]; // 13 Simbody () is 19 OpenSim
    ua[14] = u[20]; // 14 Simbody () is 20 OpenSim
    ua[15] = u[12]; // 15 Simbody () is 12 OpenSim
    ua[16] = u[13]; // 16 Simbody () is 13 OpenSim
    ua[17] = u[21]; // 17 Simbody () is 21 OpenSim
    ua[18] = u[22]; // 18 Simbody () is 22 OpenSim
    ua[19] = u[23]; // 19 Simbody () is 23 OpenSim
    ua[20] = u[24]; // 20 Simbody () is 24 OpenSim
    ua[21] = u[25]; // 21 Simbody () is 25 OpenSim
    ua[22] = u[26]; // 22 Simbody () is 26 OpenSim
    ua[23] = u[14]; // 23 Simbody () is 14 OpenSim
    ua[24] = u[15]; // 24 Simbody () is 15 OpenSim
    ua[25] = u[27]; // 25 Simbody () is 27 OpenSim
    ua[26] = u[28]; // 26 Simbody () is 28 OpenSim
    ua[27] = u[16]; // 27 Simbody () is 16 OpenSim
    ua[28] = u[17]; // 28 Simbody () is 17 OpenSim
    // pro_sup dofs are locked so we hard code the Qdotdots
    ua[29] = 0; // pro_sup is locked
    ua[30] = 0; // pro_sup is locked

    model->setStateVariableValues(*state, QsUs);
    model->realizeVelocity(*state);

    // Residual forces
    /// appliedMobilityForces (# mobilities)
    Vector appliedMobilityForces(ndofr);
    appliedMobilityForces.setToZero();
    /// appliedBodyForces (# bodies + ground)
    Vector_<SpatialVec> appliedBodyForces;
    int nbodies = model->getBodySet().getSize() + 1; // including ground
    appliedBodyForces.resize(nbodies);
    appliedBodyForces.setToZero();
    /// Gravity
    Vec3 gravity(0);
    gravity[1] = -9.81;
    /// Weight
    for (int i = 0; i < model->getBodySet().getSize(); ++i) {
        model->getMatterSubsystem().addInStationForce(*state,
            model->getBodySet().get(i).getMobilizedBodyIndex(),
            model->getBodySet().get(i).getMassCenter(),
            model->getBodySet().get(i).getMass()*gravity, appliedBodyForces);
    }
    /// Contact forces
    // Right
    Array<osim_double_adouble> Force_values_1_r = HC_1_r->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_2_r = HC_2_r->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_3_r = HC_3_r->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_4_r = HC_4_r->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_5_r = HC_5_r->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_6_r = HC_6_r->getRecordValues(*state);
    SpatialVec GRF_1_r;
    GRF_1_r[0] = Vec3(Force_values_1_r[9], Force_values_1_r[10], Force_values_1_r[11]);
    GRF_1_r[1] = Vec3(Force_values_1_r[6], Force_values_1_r[7], Force_values_1_r[8]);
    SpatialVec GRF_2_r;
    GRF_2_r[0] = Vec3(Force_values_2_r[9], Force_values_2_r[10], Force_values_2_r[11]);
    GRF_2_r[1] = Vec3(Force_values_2_r[6], Force_values_2_r[7], Force_values_2_r[8]);
    SpatialVec GRF_3_r;
    GRF_3_r[0] = Vec3(Force_values_3_r[9], Force_values_3_r[10], Force_values_3_r[11]);
    GRF_3_r[1] = Vec3(Force_values_3_r[6], Force_values_3_r[7], Force_values_3_r[8]);
    SpatialVec GRF_4_r;
    GRF_4_r[0] = Vec3(Force_values_4_r[9], Force_values_4_r[10], Force_values_4_r[11]);
    GRF_4_r[1] = Vec3(Force_values_4_r[6], Force_values_4_r[7], Force_values_4_r[8]);
    SpatialVec GRF_5_r;
    GRF_5_r[0] = Vec3(Force_values_5_r[9], Force_values_5_r[10], Force_values_5_r[11]);
    GRF_5_r[1] = Vec3(Force_values_5_r[6], Force_values_5_r[7], Force_values_5_r[8]);
    SpatialVec GRF_6_r;
    GRF_6_r[0] = Vec3(Force_values_6_r[9], Force_values_6_r[10], Force_values_6_r[11]);
    GRF_6_r[1] = Vec3(Force_values_6_r[6], Force_values_6_r[7], Force_values_6_r[8]);

    int ncalcn_r = model->getBodySet().get("calcn_r").getMobilizedBodyIndex();
    int ntoes_r = model->getBodySet().get("toes_r").getMobilizedBodyIndex();
    appliedBodyForces[ncalcn_r] = appliedBodyForces[ncalcn_r] + GRF_1_r + GRF_2_r + GRF_3_r + GRF_5_r;
    appliedBodyForces[ntoes_r] = appliedBodyForces[ntoes_r] + GRF_4_r + GRF_6_r;


    // Left
    Array<osim_double_adouble> Force_values_1_l = HC_1_l->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_2_l = HC_2_l->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_3_l = HC_3_l->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_4_l = HC_4_l->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_5_l = HC_5_l->getRecordValues(*state);
    Array<osim_double_adouble> Force_values_6_l = HC_6_l->getRecordValues(*state);
    SpatialVec GRF_1_l;
    GRF_1_l[0] = Vec3(Force_values_1_l[9], Force_values_1_l[10], Force_values_1_l[11]);
    GRF_1_l[1] = Vec3(Force_values_1_l[6], Force_values_1_l[7], Force_values_1_l[8]);
    SpatialVec GRF_2_l;
    GRF_2_l[0] = Vec3(Force_values_2_l[9], Force_values_2_l[10], Force_values_2_l[11]);
    GRF_2_l[1] = Vec3(Force_values_2_l[6], Force_values_2_l[7], Force_values_2_l[8]);
    SpatialVec GRF_3_l;
    GRF_3_l[0] = Vec3(Force_values_3_l[9], Force_values_3_l[10], Force_values_3_l[11]);
    GRF_3_l[1] = Vec3(Force_values_3_l[6], Force_values_3_l[7], Force_values_3_l[8]);
    SpatialVec GRF_4_l;
    GRF_4_l[0] = Vec3(Force_values_4_l[9], Force_values_4_l[10], Force_values_4_l[11]);
    GRF_4_l[1] = Vec3(Force_values_4_l[6], Force_values_4_l[7], Force_values_4_l[8]);
    SpatialVec GRF_5_l;
    GRF_5_l[0] = Vec3(Force_values_5_l[9], Force_values_5_l[10], Force_values_5_l[11]);
    GRF_5_l[1] = Vec3(Force_values_5_l[6], Force_values_5_l[7], Force_values_5_l[8]);
    SpatialVec GRF_6_l;
    GRF_6_l[0] = Vec3(Force_values_6_l[9], Force_values_6_l[10], Force_values_6_l[11]);
    GRF_6_l[1] = Vec3(Force_values_6_l[6], Force_values_6_l[7], Force_values_6_l[8]);
    int ncalcn_l = model->getBodySet().get("calcn_l").getMobilizedBodyIndex();
    int ntoes_l = model->getBodySet().get("toes_l").getMobilizedBodyIndex();
    appliedBodyForces[ncalcn_l] = appliedBodyForces[ncalcn_l] + GRF_1_l + GRF_2_l + GRF_3_l + GRF_5_l;
    appliedBodyForces[ntoes_l] = appliedBodyForces[ntoes_l] + GRF_4_l + GRF_6_l;
    // Ground reaction forces
    SpatialVec GRF_r = GRF_1_r + GRF_2_r + GRF_3_r + GRF_4_r + GRF_5_r + GRF_6_r;
    SpatialVec GRF_l = GRF_1_l + GRF_2_l + GRF_3_l + GRF_4_l + GRF_5_l + GRF_6_l;

    /// knownUdot
    Vector knownUdot(ndofr);
    knownUdot.setToZero();
    for (int i = 0; i < ndofr; ++i) knownUdot[i] = ua[i];

    // Residual forces
    Vector residualMobilityForces(ndofr);
    residualMobilityForces.setToZero();
    model->getMatterSubsystem().calcResidualForceIgnoringConstraints(*state,
        appliedMobilityForces, appliedBodyForces, knownUdot,
        residualMobilityForces);

    // Extract the origin of the calcaneus to impose constraint in ocp
    Vec3 calcn_or_l  = calcn_l->getPositionInGround(*state);
    Vec3 calcn_or_r  = calcn_r->getPositionInGround(*state);

     //CasADi may not always request all outputs
     //if res[i] is a null pointer, this means that output i is not required
    int nc = 3; // # components in Vec3
    if (res[0]) {
        for (int i = 0; i < 12; ++i) {
            res[0][i] = value<T>(residualMobilityForces[i]); // joint torques
        }
        // order adjusted since order Simbody is different than order OpenSim
        res[0][12] = value<T>(residualMobilityForces[15]);
        res[0][13] = value<T>(residualMobilityForces[16]);
        res[0][14] = value<T>(residualMobilityForces[23]);
        res[0][15] = value<T>(residualMobilityForces[24]);
        res[0][16] = value<T>(residualMobilityForces[27]);
        res[0][17] = value<T>(residualMobilityForces[28]);
        res[0][18] = value<T>(residualMobilityForces[12]);
        res[0][19] = value<T>(residualMobilityForces[13]);
        res[0][20] = value<T>(residualMobilityForces[14]);
        res[0][21] = value<T>(residualMobilityForces[17]);
        res[0][22] = value<T>(residualMobilityForces[18]);
        res[0][23] = value<T>(residualMobilityForces[19]);
        res[0][24] = value<T>(residualMobilityForces[20]);
        res[0][25] = value<T>(residualMobilityForces[21]);
        res[0][26] = value<T>(residualMobilityForces[22]);
        res[0][27] = value<T>(residualMobilityForces[25]);
        res[0][28] = value<T>(residualMobilityForces[26]);
        for (int i = 0; i < nc; ++i) {
            res[0][i + ndof] = value<T>(GRF_r[1][i]); // GRF_r
        }
        for (int i = 0; i < nc; ++i) {
            res[0][i + ndof + nc] = value<T>(GRF_l[1][i]); // GRF_l
        }
        for (int i = 0; i < nc; ++i) {
            res[0][i + ndof + nc + nc] = value<T>(calcn_or_r[i]); // calcn_or_r
        }
        for (int i = 0; i < nc; ++i) {
            res[0][i + ndof + nc + nc + nc] = value<T>(calcn_or_l[i]); // calcn_or_l
        }
    }
    return 0;
}

int main() {

    Recorder x[NX];
    Recorder u[NU];
    Recorder tau[NR];

    for (int i = 0; i < NX; ++i) x[i] <<= 0;
    for (int i = 0; i < NU; ++i) u[i] <<= 0;

    const Recorder* Recorder_arg[n_in] = { x,u };
    Recorder* Recorder_res[n_out] = { tau };

    F_generic<Recorder>(Recorder_arg, Recorder_res);

    double res[NR];
    for (int i = 0; i < NR; ++i) Recorder_res[0][i] >>= res[i];

    Recorder::stop_recording();

    return 0;

}