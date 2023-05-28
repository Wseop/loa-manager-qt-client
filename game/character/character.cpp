#include "character.h"

Character::Character()
{

}

Profile Character::profile() const
{
    return mProfile;
}

void Character::setProfile(const Profile &newProfile)
{
    mProfile = newProfile;
}
