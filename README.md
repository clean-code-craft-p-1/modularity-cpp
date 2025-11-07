# Modularity Challenge

Welcome to the art of breaking things apart! 🧩

## Your Mission

You have inherited a temperature data processor that does everything in one big file. While it works now, your team will grow and everyone needs to modify different parts of the same file. That's something you need to avoid.

Your job: **Transform this into a well-organized, modular codebase** where developers can work independently without stepping on each other's toes.

The CI pipeline will guide you with automatic checks, so let the red/green feedback be your friend!

## Modularity motivation

As you refactor, keep in mind that successful software evolves. Your customers will keep asking for new ways to analyze temperature data. Here are some examples of what might be coming:

### 🚨 Feature 1: Fever Detection & Alerting

"As a caregiver, I want to be alerted if a patient's body temperature sustains high for more than 30 minutes, so that I can intervene quickly."

This feature will need frequent updates as medical protocols change.

### 📊 Feature 2: Circadian Pattern Summary

"As a caregiver, I want to review 24-hour patterns to check for abnormal rhythms (e.g., fever at night but not daytime)."

This will grow with new analytics and trend insights.

## The goal

Can you structure your code so that future developers can work on these new features independently, end-to-end, without conflicts?

Happy refactoring! 🎯
