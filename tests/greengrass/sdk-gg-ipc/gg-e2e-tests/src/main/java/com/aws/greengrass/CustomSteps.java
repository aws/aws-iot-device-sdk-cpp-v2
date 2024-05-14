package com.aws.greengrass;

import io.cucumber.guice.ScenarioScoped;
import io.cucumber.java.en.And;

/**
 * Change this or create a file like this one to define custom steps to test functionality
 * specific to your component
 */
@ScenarioScoped
public class CustomSteps {

    @And("I call my custom step")
    public void customStep() {
        System.out.println("My custom step was called ");
    }
}