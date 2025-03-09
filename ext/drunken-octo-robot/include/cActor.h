/*
 * cActor.h
 *
 *  Created on: 2012-05-24
 *      Author: dri
 *
 * This class is meant to emulate the capabilities of python Actor class. It is
 * by no mean a full implementation. It only contains what is needed to
 * translate the Panda3D tutorials.
 *
 *  Updated on: 2025-03-06
 *      Author: tomatoes
 *     Changes: Fixed compile with latest version of panda3d (1.10.0) on Arch
 * Linux; Adjusted formatting to my preference; Split sources and headers to
 * seoerate directories; Meson Build System Support
 *
 */

#ifndef CACTOR_H_
#define CACTOR_H_

#include <pandaFramework.h>

using std::endl;
using std::string;
class CActor : public NodePath, public AnimControlCollection
{
  public:
    typedef pvector<string>       NameVec;
    typedef pmap<string, NameVec> AnimMap;

    CActor();
    virtual ~CActor();

    /**
     *   This function loads an actor model and its animations, letting the user
     * specify the name of each animation. The actor will be parented to the
     * framework's models node (to render the actor, reparent it to the render
     * node)
     *
     *   @param windowFrameworkPtr the WindowFramework for this actor
     *   @param actorFilename  the actor's egg file
     *   @param animMap        a map of the desired name (first) of each
     * animation and its associated file (second). Set this parameter to NULL if
     * there are no animations to bind to the actor.
     *   @param hierarchyMatchFlags: idem as the same parameter from
     * auto_bind().
     */
    void load_actor(
        WindowFramework* windowFrameworkPtr,
        const string&    actorFilename,
        const AnimMap*   animMapPtr,
        int              hierarchyMatchFlags
    );

    NodePath control_joint(const string& jointName);
    NodePath expose_joint(const string& jointName);

  private:
    typedef pvector<NodePath> NodePathVec;

    void load_anims(
        const AnimMap* animMapPtr,
        const string&  filename,
        int            hierarchyMatchFlags
    );

    bool is_stored(const AnimControl* animPtr);
};

#endif /* CACTOR_H_ */
