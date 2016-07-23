#include "stdafx.h"
#include "catch.hpp"
#include "Application.h"
#include <rapidxml_utils.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

bool unitTests = true;

namespace DissertationUnitTests {
  void integrationUnitTests(int particleNumber, const Particle& particle);

  TEST_CASE("Test cloth XML loading") {
    Application a;

    file<> file("..\\DissertationUnitTests\\test_params.xml");
    xml_document<> doc;
    doc.parse<0>(file.data());
    xml_node<>* rootNode = doc.first_node();
    xml_node<>* currentTest = rootNode->first_node();

    a.loadTest(currentTest, SHEET);

    const Cloth* const cloth = a.getCloth();

    REQUIRE(cloth->getNumRows() == 3);
    REQUIRE(cloth->getNumColumns() == 3);
    
    const Particle* const particles = cloth->getParticles();

    REQUIRE(particles[0].getPosition().m128_f32[0] == -10);
    REQUIRE(particles[0].getPosition().m128_f32[1] == 10);
    REQUIRE(particles[0].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[1].getPosition().m128_f32[0] == -6);
    REQUIRE(particles[1].getPosition().m128_f32[1] == 10);
    REQUIRE(particles[1].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[2].getPosition().m128_f32[0] == -2);
    REQUIRE(particles[2].getPosition().m128_f32[1] == 10);
    REQUIRE(particles[2].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[3].getPosition().m128_f32[0] == -10);
    REQUIRE(particles[3].getPosition().m128_f32[1] == 6);
    REQUIRE(particles[3].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[4].getPosition().m128_f32[0] == -6);
    REQUIRE(particles[4].getPosition().m128_f32[1] == 6);
    REQUIRE(particles[4].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[5].getPosition().m128_f32[0] == -2);
    REQUIRE(particles[5].getPosition().m128_f32[1] == 6);
    REQUIRE(particles[5].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[6].getPosition().m128_f32[0] == -10);
    REQUIRE(particles[6].getPosition().m128_f32[1] == 2);
    REQUIRE(particles[6].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[7].getPosition().m128_f32[0] == -6);
    REQUIRE(particles[7].getPosition().m128_f32[1] == 2);
    REQUIRE(particles[7].getPosition().m128_f32[2] == 10);

    REQUIRE(particles[8].getPosition().m128_f32[0] == -2);
    REQUIRE(particles[8].getPosition().m128_f32[1] == 2);
    REQUIRE(particles[8].getPosition().m128_f32[2] == 10);
  }

  TEST_CASE("Test integration") {
    SECTION("Test explicit Euler") {
      Application a;

      file<> file("..\\DissertationUnitTests\\explicit_euler_test_params.xml");
      xml_document<> doc;
      doc.parse<0>(file.data());
      xml_node<>* rootNode = doc.first_node();
      xml_node<>* currentTest = rootNode->first_node();

      INFO("Explicit Euler");
      loadUnitTestData("..\\DissertationUnitTests\\explicit_euler_step_1_unit_test_data.txt");
      a.loadTest(currentTest, SHEET);
      a.update(1001.0);

      Particle* particles = a.getCloth()->getParticles();
     
      for (int i = 0; i < a.getCloth()->getNumRows(); i++) {
        for (int j = 0; j < a.getCloth()->getNumColumns(); j++) {
          if (!particles[(i * a.getCloth()->getNumColumns()) + j].isPinned()) {
            integrationUnitTests((i * a.getCloth()->getNumColumns()) + j, particles[(i * a.getCloth()->getNumColumns()) + j]);
            particles[(i * a.getCloth()->getNumColumns()) + j].zeroForce();
          }
        }
      }

      unitTestData.clear();
      loadUnitTestData("..\\DissertationUnitTests\\explicit_euler_step_2_unit_test_data.txt");
      a.update(1001.0);

      particles = a.getCloth()->getParticles();

      for (int i = 0; i < a.getCloth()->getNumRows(); i++) {
        for (int j = 0; j < a.getCloth()->getNumColumns(); j++) {
          if (!particles[(i * a.getCloth()->getNumColumns()) + j].isPinned()) {
            integrationUnitTests((i * a.getCloth()->getNumColumns()) + j, particles[(i * a.getCloth()->getNumColumns()) + j]);
            particles[(i * a.getCloth()->getNumColumns()) + j].zeroForce();
          }
        }
      }
    }

    SECTION("Test Verlet") {
      Application a;

      file<> file("..\\DissertationUnitTests\\verlet_test_params.xml");
      xml_document<> doc;
      doc.parse<0>(file.data());
      xml_node<>* rootNode = doc.first_node();
      xml_node<>* currentTest = rootNode->first_node();

      INFO("Verlet");
      unitTestData.clear();
      loadUnitTestData("..\\DissertationUnitTests\\verlet_step_1_unit_test_data.txt");
      a.loadTest(currentTest, SHEET);
      a.update(1001.0);

      Particle* particles = a.getCloth()->getParticles();

      for (int i = 0; i < a.getCloth()->getNumRows(); i++) {
        for (int j = 0; j < a.getCloth()->getNumColumns(); j++) {
          if (!particles[(i * a.getCloth()->getNumColumns()) + j].isPinned()) {
            integrationUnitTests((i * a.getCloth()->getNumColumns()) + j, particles[(i * a.getCloth()->getNumColumns()) + j]);
            particles[(i * a.getCloth()->getNumColumns()) + j].zeroForce();
          }
        }
      }

      unitTestData.clear();
      loadUnitTestData("..\\DissertationUnitTests\\verlet_step_2_unit_test_data.txt");
      a.update(1001.0);

      particles = a.getCloth()->getParticles();

      for (int i = 0; i < a.getCloth()->getNumRows(); i++) {
        for (int j = 0; j < a.getCloth()->getNumColumns(); j++) {
          if (!particles[(i * a.getCloth()->getNumColumns()) + j].isPinned()) {
            integrationUnitTests((i * a.getCloth()->getNumColumns()) + j, particles[(i * a.getCloth()->getNumColumns()) + j]);
            particles[(i * a.getCloth()->getNumColumns()) + j].zeroForce();
          }
        }
      }
    }

    SECTION("Test Midpoint") {
      Application a;

      file<> file("..\\DissertationUnitTests\\midpoint_test_params.xml");
      xml_document<> doc;
      doc.parse<0>(file.data());
      xml_node<>* rootNode = doc.first_node();
      xml_node<>* currentTest = rootNode->first_node();

      INFO("Midpoint");
      unitTestData.clear();
      loadUnitTestData("..\\DissertationUnitTests\\midpoint_unit_test_data.txt");
      a.loadTest(currentTest, SHEET);
      a.update(1001.0);

      Particle* particles = a.getCloth()->getParticles();

      for (int i = 0; i < a.getCloth()->getNumRows(); i++) {
        for (int j = 0; j < a.getCloth()->getNumColumns(); j++) {
          if (!particles[(i * a.getCloth()->getNumColumns()) + j].isPinned()) {
            integrationUnitTests((i * a.getCloth()->getNumColumns()) + j, particles[(i * a.getCloth()->getNumColumns()) + j]);
            particles[(i * a.getCloth()->getNumColumns()) + j].zeroForce();
          }
        }
      }
    }

    SECTION("Test RK4") {
      Application a;

      file<> file("..\\DissertationUnitTests\\rk4_test_params.xml");
      xml_document<> doc;
      doc.parse<0>(file.data());
      xml_node<>* rootNode = doc.first_node();
      xml_node<>* currentTest = rootNode->first_node();

      INFO("RK4");
      unitTestData.clear();
      loadUnitTestData("..\\DissertationUnitTests\\rk4_unit_test_data.txt");
      a.loadTest(currentTest, SHEET);
      a.update(1001.0);

      Particle* particles = a.getCloth()->getParticles();

      for (int i = 0; i < a.getCloth()->getNumRows(); i++) {
        for (int j = 0; j < a.getCloth()->getNumColumns(); j++) {
          if (!particles[(i * a.getCloth()->getNumColumns()) + j].isPinned()) {
            integrationUnitTests((i * a.getCloth()->getNumColumns()) + j, particles[(i * a.getCloth()->getNumColumns()) + j]);
            particles[(i * a.getCloth()->getNumColumns()) + j].zeroForce();
          }
        }
      }
    }
  }

  void integrationUnitTests(int particleNumber, const Particle& particle) {
    UnitTestData data = unitTestData[particleNumber];

    INFO("particle " << particleNumber);

    INFO("particle force (" << particle.getTotalForce().m128_f32[0] << "," << particle.getTotalForce().m128_f32[1] << "," << particle.getTotalForce().m128_f32[2] << ")");
    INFO("map force (" << data.totalForce.m128_f32[0] << "," << data.totalForce.m128_f32[1] << "," << data.totalForce.m128_f32[2] << ")");

    INFO("particle velocity (" << particle.getVelocity().m128_f32[0] << "," << particle.getVelocity().m128_f32[1] << "," << particle.getVelocity().m128_f32[2] << ")");
    INFO("map velocity (" << data.velocity.m128_f32[0] << "," << data.velocity.m128_f32[1] << "," << data.velocity.m128_f32[2] << ")");

    INFO("particle position (" << particle.getPosition().m128_f32[0] << "," << particle.getPosition().m128_f32[1] << "," << particle.getPosition().m128_f32[2] << ")");
    INFO("map position (" << data.position.m128_f32[0] << "," << data.position.m128_f32[1] << "," << data.position.m128_f32[2] << ")");

    REQUIRE(abs(abs(particle.getTotalForce().m128_f32[0]) - abs(data.totalForce.m128_f32[0])) <= 0.001f);
    REQUIRE(abs(abs(particle.getTotalForce().m128_f32[1]) - abs(data.totalForce.m128_f32[1])) <= 0.001f);
    REQUIRE(abs(abs(particle.getTotalForce().m128_f32[2]) - abs(data.totalForce.m128_f32[2])) <= 0.001f);

    REQUIRE(abs(abs(particle.getVelocity().m128_f32[0]) - abs(data.velocity.m128_f32[0])) <= 0.001f);
    REQUIRE(abs(abs(particle.getVelocity().m128_f32[1]) - abs(data.velocity.m128_f32[1])) <= 0.001f);
    REQUIRE(abs(abs(particle.getVelocity().m128_f32[2]) - abs(data.velocity.m128_f32[2])) <= 0.001f);

    REQUIRE(abs(abs(particle.getPosition().m128_f32[0]) - abs(data.position.m128_f32[0])) <= 0.001f);
    REQUIRE(abs(abs(particle.getPosition().m128_f32[1]) - abs(data.position.m128_f32[1])) <= 0.001f);
    REQUIRE(abs(abs(particle.getPosition().m128_f32[2]) - abs(data.position.m128_f32[2])) <= 0.001f);
  }
}